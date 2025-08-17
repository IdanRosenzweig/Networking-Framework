#include "nsp_protocol.h"

#include <iostream>
#include <cstring>
using namespace std;


void nsp_protocol::send(shared_ptr<net_access> const& nsp_surface, nsp_header const& header, vector<uint8_t> const& data) {
    if (nsp_surface == nullptr) return;

    uint8_t big_buff[0x1000];

    // nsp header
    auto header_sz = write_in_network_order(big_buff, &header);

    // encapsulated data
    memcpy(big_buff + header_sz, data.data(), data.size());

    nsp_surface->get_send_access()->send_data(vector<uint8_t>(big_buff, big_buff + header_sz + data.size()));
}

void nsp_protocol::connect_recv(
    shared_ptr<net_access> const& nsp_surface, shared_ptr<recv_listener<pair<nsp_header, vector<uint8_t>>>> const& recv,
    optional<nsp_id> src_id, optional<nsp_id> dest_id
) {
    if (nsp_surface == nullptr) return;

    struct my_recv : public recv_listener_bytes {
        shared_ptr<recv_listener<pair<nsp_header, vector<uint8_t>>>> recv;
        optional<nsp_id> src_id;
        optional<nsp_id> dest_id;
        my_recv(shared_ptr<recv_listener<pair<nsp_header, vector<uint8_t>>>> const& recv, optional<nsp_id> src_id, optional<nsp_id> dest_id) : recv(recv), src_id(src_id), dest_id(dest_id) {}

        void handle_recv(vector<uint8_t> const& data) override {
            nsp_header header;
            auto header_sz = extract_from_network_order(&header, data.data());

            vector<uint8_t> encap_data(data.data() + header_sz, data.data() + data.size());

            // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
            if (src_id.has_value()) {
                if (header.src_id != src_id.value()) {
                    // cout << "src id no match, needed " << src_id.value() << " got " << header.src_id << endl;
                    return;
                }
            }

            if (dest_id.has_value()) {
                if (header.dest_id != dest_id.value()) {
                    // cout << "dest id no match, needed " << dest_id.value() << " got " << header.dest_id << endl;
                    return;
                }
            }

            recv->handle_recv({header, encap_data});
        }
    };

    nsp_surface->set_recv_access(make_shared<my_recv>(recv, src_id, dest_id));
}


void nsp_protocol::connect_net_access_generator_listener(shared_ptr<net_access> const& access, nsp_id dest_id, shared_ptr<net_access_generator_listener>&& listener) {
    struct my_recv : public recv_listener<pair<nsp_header, vector<uint8_t>>> {
        nsp_id dest_id;
        multi_net_access multi_access;
        shared_ptr<net_access_generator_listener> listener;

        my_recv(nsp_id dest_id, shared_ptr<net_access> const& access, shared_ptr<net_access_generator_listener>&& listener) : dest_id(dest_id), multi_access(std::move(shared_ptr<net_access>(access))), listener(std::move(listener)) {
        }

        void handle_recv(pair<nsp_header, vector<uint8_t>> const& data) {
            // cout << "nsp generator called" << endl;
            if (listener == nullptr) {cout << "won't generate new net access, because there is no listener" << endl;return;}

            // create a new net access
            auto new_net_access = make_shared<nsp_protocol::net_access_single>(std::move(multi_access.generate_net_access()), data.first.src_id, dest_id);

            // forward it to the generator
            listener->handle_new_net_access(new_net_access);
            
            // send the data
            // cout << "nsp generator handles data" << endl;
            if (new_net_access->recv_access != nullptr) {
                // cout << "nsp generator sends data" << endl;
                new_net_access->recv_access->handle_recv(data.second); // todo fix it is possible that another data will be received into new_net_access async in the meantime, before this call is invoked
            }
        }
    };

    nsp_protocol::connect_recv(access, make_shared<my_recv>(dest_id, access, std::move(listener)), {}, dest_id);
}