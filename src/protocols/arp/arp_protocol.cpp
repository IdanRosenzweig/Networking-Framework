#include "arp_protocol.h"

#include <cstdio>

void arp_protocol::send() {
    // write to buff
    vector<uint8_t> buff(next_header.calc_sz());
    write_in_network_order(buff.data(), &next_header);

    net_access->get_send_access()->send_data(buff);
}