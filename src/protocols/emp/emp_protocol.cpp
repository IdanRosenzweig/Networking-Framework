#include "emp_protocol.h"

#include <iostream>
#include <cstring>
using namespace std;


void emp_protocol::send(shared_ptr<net_access> const& emp_surface, emp_header const& header, vector<uint8_t> const& data) {
    if (emp_surface == nullptr) return;

    uint8_t big_buff[0x1000];

    // emp header
    auto header_sz = write_in_network_order(big_buff, &header);

    // encapsulated data
    memcpy(big_buff + header_sz, data.data(), data.size());

    emp_surface->get_send_access()->send_data(vector<uint8_t>(big_buff, big_buff + header_sz + data.size()));
}

void emp_protocol::connect_recv(
    shared_ptr<net_access> const& emp_surface, shared_ptr<recv_listener<pair<emp_header, vector<uint8_t>>>> const& recv,
    optional<emp_port> src_port, optional<emp_port> dest_port
) {
    if (emp_surface == nullptr) return;

    struct my_recv : public recv_listener_bytes {
        shared_ptr<recv_listener<pair<emp_header, vector<uint8_t>>>> recv;
        optional<emp_port> src_port;
        optional<emp_port> dest_port;
        my_recv(shared_ptr<recv_listener<pair<emp_header, vector<uint8_t>>>> const& recv, optional<emp_port> src_port, optional<emp_port> dest_port) : recv(recv), src_port(src_port), dest_port(dest_port) {}

        void handle_recv(vector<uint8_t> const& data) override {
            emp_header header;
            auto header_sz = extract_from_network_order(&header, data.data());

            vector<uint8_t> encap_data(data.data() + header_sz, data.data() + data.size());

            // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
            if (src_port.has_value()) {
                if (header.src_port != src_port.value()) {
                    // cout << "src port no match, needed " << src_port.value() << " got " << header.src_port << endl;
                    return;
                }
            }

            if (dest_port.has_value()) {
                if (header.dest_port != dest_port.value()) {
                    // cout << "dest port no match, needed " << dest_port.value() << " got " << header.dest_port << endl;
                    return;
                }
            }

            recv->handle_recv({header, encap_data});
        }
    };

    emp_surface->set_recv_access(make_shared<my_recv>(recv, src_port, dest_port));
}


void emp_protocol::connect_net_access_generator_listener(shared_ptr<net_access> const& access, emp_port dest_port, shared_ptr<net_access_generator_listener>&& listener) {
    struct my_recv : public recv_listener<pair<emp_header, vector<uint8_t>>> {
        emp_port dest_port;
        multi_net_access multi_access;
        shared_ptr<net_access_generator_listener> listener;

        my_recv(emp_port dest_port, shared_ptr<net_access> const& access, shared_ptr<net_access_generator_listener>&& listener) : dest_port(dest_port), multi_access(std::move(shared_ptr<net_access>(access))), listener(std::move(listener)) {
        }

        void handle_recv(pair<emp_header, vector<uint8_t>> const& data) {
            // cout << "emp generator called" << endl;
            if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

            // create a new net access
            auto new_net_access = make_shared<emp_protocol::net_access_single>(std::move(multi_access.generate_net_access()), data.first.src_port, dest_port);

            // forward it to the generator
            listener->handle_new_net_access(new_net_access);
            
            // send the data
            // cout << "emp generator handles data" << endl;
            if (new_net_access->recv_access != nullptr) {
                // cout << "emp generator sends data" << endl;
                new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
            }
        }
    };

    emp_protocol::connect_recv(access, make_shared<my_recv>(dest_port, access, std::move(listener)), {}, dest_port);
}