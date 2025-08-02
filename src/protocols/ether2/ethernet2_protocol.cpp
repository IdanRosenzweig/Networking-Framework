#include "ethernet2_protocol.h"

#include <cstring>
using namespace std;

namespace ethernet2_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, ethernet2_header const& header, vector<uint8_t> const& data) {
        if (net_access == nullptr) return;

        vector<uint8_t> buff(sizeof(ethernet2_header) + data.size()/* + 4*/);

        // ethernet header
        write_in_network_order(buff.data(), &header);

        // encapsulated data
        memcpy(buff.data() + sizeof(struct ethernet2_header), data.data(), data.size());

        // crc
        // *(uint32_t*)(buff.data() + sizeof(struct ethernet2_header) + data.size()) = ethernet_calc_crc(buff.data(), sizeof(ethernet2_header) + data.size());

        net_access->get_send_access()->send_data(buff);
    }

    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>>> const& recv,
        optional<mac_addr> src_addr, optional<mac_addr> dest_addr, optional<ethertype> type
    ) {
        if (net_access == nullptr) return;

        struct my_recv : public basic_recv_listener<vector<uint8_t>> {
            shared_ptr<basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>>> recv;
            optional<mac_addr> src_addr;
            optional<mac_addr> dest_addr;
            optional<ethertype> type;
            my_recv(shared_ptr<basic_recv_listener<pair<ethernet2_header, vector<uint8_t>>>> const& recv, optional<mac_addr> src_addr, optional<mac_addr> dest_addr, optional<ethertype> type) : recv(recv), src_addr(src_addr), dest_addr(dest_addr), type(type) {}

            void handle_recv(vector<uint8_t> const& data) override {
                ethernet2_header eth_header;
                extract_from_network_order(&eth_header, data.data());

                vector<uint8_t> encap_data(data.data() + sizeof(ethernet2_header), data.data() + data.size());

                // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
                if (src_addr.has_value()) {
                    if (eth_header.src_addr != src_addr.value()) {return;}
                }

                if (dest_addr.has_value()) {
                    if (eth_header.dest_addr != dest_addr.value()) return;
                }

                if (type.has_value()) {
                    if (eth_header.prot != type.value()) return;
                }

                recv->handle_recv({eth_header, encap_data});
            }
        };

        net_access->set_recv_access(make_shared<my_recv>(recv, src_addr, dest_addr, type));
    }

}
