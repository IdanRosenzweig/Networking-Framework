#include "arp_protocol.h"

#include <cstdio>

namespace arp_protocol {
    void send(shared_ptr<net_access> const& net_access, arp_header const& header) {
        vector<uint8_t> buff(sizeof(arp_header));

        // arp header
        write_in_network_order(buff.data(), &header);

        net_access->get_send_access()->send_data(buff);
    }

    void connect_recv(
        shared_ptr<net_access> const& net_access, shared_ptr<recv_listener<arp_header>> const& recv,
        optional<arp_op_values> op
    ) {
        struct my_recv : public recv_listener_bytes {
            shared_ptr<recv_listener<arp_header>> recv;
            optional<arp_op_values> op;
            my_recv(shared_ptr<recv_listener<arp_header>> const& recv, optional<arp_op_values> op) : recv(recv), op(op) {}

            void handle_recv(vector<uint8_t> const& data) override {
                struct arp_header header;
                extract_from_network_order(&header, data.data());

                if (op.has_value()) {
                    if (header.op != op.value()) return;
                }

                recv->handle_recv(header);
            }
        };

        net_access->set_recv_access(make_shared<my_recv>(recv, op));
    }

}
