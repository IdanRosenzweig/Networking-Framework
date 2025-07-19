#include "ethernet2_protocol.h"

#include <cstring>
using namespace std;

void ethernet2_protocol::send(vector<uint8_t> const& data) {
    vector<uint8_t> buff(sizeof(ethernet_header) + data.size()/* + 4*/);

    // ethernet header
    struct ethernet_header header;

    if (!next_dest_addr.has_value()) return;
    header.dest_addr = next_dest_addr.value();

    if (!next_source_addr.has_value()) return;
    header.source_addr = next_source_addr.value();
    
    if (!next_protocol.has_value()) return;
    header.ether_type = next_protocol.value();
    
    write_in_network_order(buff.data(), &header);

    // encapsulated data
    memcpy(buff.data() + sizeof(struct ethernet_header), data.data(), data.size());

    // crc
    // *(uint32_t*)(buff.data() + sizeof(struct ethernet_header) + data.size()) = ethernet_calc_crc(buff.data(), sizeof(ethernet_header) + data.size());

    net_access->get_send_access()->send_data(buff);
}
