#include "ethernet2_protocol.h"

#include <cstring>
using namespace std;

namespace ethernet2_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, ethernet_header const& header, vector<uint8_t> const& data) {
        vector<uint8_t> buff(sizeof(ethernet_header) + data.size()/* + 4*/);

        // ethernet header        
        write_in_network_order(buff.data(), &header);

        // encapsulated data
        memcpy(buff.data() + sizeof(struct ethernet_header), data.data(), data.size());

        // crc
        // *(uint32_t*)(buff.data() + sizeof(struct ethernet_header) + data.size()) = ethernet_calc_crc(buff.data(), sizeof(ethernet_header) + data.size());

        net_access->get_send_access()->send_data(buff);
    }

    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<encap_prot_listener<ethernet_header>> const recv,
        optional<mac_addr> src_mac, optional<mac_addr> dest_mac, optional<ethertype> type
    ) {
        struct my_recv : public basic_recv_listener<vector<uint8_t>> {
            shared_ptr<encap_prot_listener<ethernet_header>> recv;
            optional<mac_addr> src_mac;
            optional<mac_addr> dest_mac;
            optional<ethertype> type;
            my_recv(shared_ptr<encap_prot_listener<ethernet_header>> const& recv, optional<mac_addr> src_mac, optional<mac_addr> dest_mac, optional<ethertype> type) : recv(recv), src_mac(src_mac), dest_mac(dest_mac), type(type) {}

            void handle_recv(vector<uint8_t> const& data) override {
                shared_ptr<ethernet_header> eth_header = make_shared<ethernet_header>();
                extract_from_network_order(eth_header.get(), data.data());

                vector<uint8_t> encap_data(data.data() + sizeof(ethernet_header), data.data() + data.size());

                // todo instead of checking all parameters, do something smarter ("interuupt" based on the parameters)
                if (src_mac.has_value()) {
                    if (eth_header->source_addr != src_mac.value()) return;
                }

                if (dest_mac.has_value()) {
                    if (eth_header->dest_addr != dest_mac.value()) return;
                }

                if (type.has_value()) {
                    if (eth_header->ether_type != type.value()) return;
                }

                recv->handle_recv({eth_header, encap_data});
            }
        };

        net_access->set_recv_access(make_shared<my_recv>(recv, src_mac, dest_mac, type));
    }

}
