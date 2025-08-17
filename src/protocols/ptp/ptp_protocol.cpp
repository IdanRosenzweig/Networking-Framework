#include "ptp_protocol.h"

namespace ptp_protocol {
    vector<uint8_t> write_packet(ptp_header const& header, vector<uint8_t> const& data) {
        thread_local static uint8_t buff[0x10000];

        // ptp header
        auto header_sz = write_in_network_order(buff, &header);

        // encapsulated data
        memcpy(buff + header_sz, data.data(), data.size());

        return vector<uint8_t>(buff, buff + header_sz + data.size());
    }

    void send(shared_ptr<net_access_send> const& ptp_send_surface, ptp_header const& header, vector<uint8_t> const& data) {
        if (ptp_send_surface == nullptr) return;

        // send
        ptp_send_surface->get_send_access()->send_data(
            ptp_protocol::write_packet(header, data)
        );
    }

    shared_ptr<send_medium_bytes> create_send_access_from_routing_table(ptp_local_conn_routing_table&& routing_table) {
        // todo take shared_ptr<routing_table> so that the table can be updated online
        struct my_send : public send_medium_bytes {
            ptp_local_conn_routing_table routing_table;
            my_send(ptp_local_conn_routing_table&& routing_table) : routing_table(std::move(routing_table)) {
            }

            err_t send_data(vector<uint8_t> const& data) override {
                struct ptp_header header;
                extract_from_network_order(&header, data.data());

                if (header.get_curr_action().type != ptp_action_t::local_conn) return err_t::ERR;

                ptp_local_conn local_conn = std::get<ptp_local_conn>(header.get_curr_action().val);

                auto route = routing_table.find_route(local_conn.dest_addr);
                if (route == nullptr) return err_t::ERR;

                return route->get_send_access()->send_data(data);
            }
        };

        return make_shared<my_send>(std::move(routing_table));
    }

    void connect_recv(
        shared_ptr<net_access_recv> const& ptp_recv_surface, shared_ptr<recv_listener<pair<ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> const& recv,
        function<bool(ptp_header const&)> cond
    ) {
        if (ptp_recv_surface == nullptr) return;

        struct my_recv : public recv_listener_bytes {
            shared_ptr<recv_listener<pair<ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> recv;
            function<bool(ptp_header const&)> cond;
            my_recv(shared_ptr<recv_listener<pair<ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> const& _recv, function<bool(ptp_header const&)> const& _cond) : recv(_recv), cond(_cond) {
            }

            void handle_recv(vector<uint8_t> const& data) override {
                ptp_header header;
                auto header_sz = extract_from_network_order(&header, data.data());

                // match condition
                if (!cond(header)) {
                    cout << "ptp cond didn't match" << endl;
                    return;
                }
                cout << "ptp cond matched" << endl; 

                vector<uint8_t> encap_data(data.data() + header_sz, data.data() + data.size());

                recv->handle_recv({header, encap_data});
            }
        };

        ptp_recv_surface->set_recv_access(make_shared<my_recv>(recv, cond));
    }

    void connect_recv_with_original(
        shared_ptr<net_access_recv> const& ptp_recv_surface, shared_ptr<recv_listener<tuple<vector<uint8_t>/*full original data*/, ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> const& recv,
        function<bool(ptp_header const&)> cond
    ) {
        if (ptp_recv_surface == nullptr) return;

        struct my_recv : public recv_listener_bytes {
            shared_ptr<recv_listener<tuple<vector<uint8_t>/*full original data*/, ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> recv;
            function<bool(ptp_header const&)> cond;
            my_recv(shared_ptr<recv_listener<tuple<vector<uint8_t>/*full original data*/, ptp_header/*ptp header*/, vector<uint8_t>/*encapsulated data*/>>> const& _recv, function<bool(ptp_header const&)> const& _cond) : recv(_recv), cond(_cond) {
            }

            void handle_recv(vector<uint8_t> const& data) override {
                ptp_header header;
                auto header_sz = extract_from_network_order(&header, data.data());

                // match condition
                if (!cond(header)) {cout << "ptp cond didn't match" << endl; return;}
                cout << "ptp cond matched" << endl; 

                vector<uint8_t> encap_data(data.data() + header_sz, data.data() + data.size());

                recv->handle_recv({data, header, encap_data});
            }
        };

        ptp_recv_surface->set_recv_access(make_shared<my_recv>(recv, cond));
    }

}

namespace ptp_protocol {
    void connect_net_access_generator_listener(
        shared_ptr<net_access_recv> const& ptp_recv_surface,
        shared_ptr<net_access_send> const& ptp_send_surface,
        // prot
        shared_ptr<net_access_generator_listener>&& listener
    ) {
        auto splits = multi_net_access_recv_split(ptp_recv_surface, 2);
        auto base_recv = splits[0];
        auto generate_recv = splits[1];

        ptp_protocol::connect_recv(base_recv, make_shared<my_net_access_generator_listener>(generate_recv, ptp_send_surface, std::move(listener)),
            [](ptp_header const& header) -> bool {
                return ptp_is_endpoint_end(header);
            }
        );
    }
}