#pragma once

#include <memory>
#include <vector>
#include <utility>
using namespace std;

#include "src/protocols/ether2/ethernet2_protocol.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/protocols/ether2/mac_addr.h"
#include "src/protocols/ip4/ip4_addr.h"

namespace arp_tool {
    mac_addr search_for_mac_addr(shared_ptr<net_access> const& arp_broadcast_surface, ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip);

    map<ip4_addr, mac_addr> scan_entire_subnet(shared_ptr<net_access> const& arp_broadcast_surface, ip4_subnet_mask mask, set<ip4_addr> const& skip_addrs, mac_addr src_mac, ip4_addr src_ip);

    void announce_new_pair(shared_ptr<net_access> const& arp_broadcast_surface, ip4_addr new_ip, mac_addr new_mac);

    // targets: list of victim targets, contains a net_access (only send access is needed, recv access unused) and the associated mac addr and ip addr to be used within arp
    // dest: intercept communication from the targets to the dest
    // src_mac and src_ip: new dest for the victims, in place of the old dest
    // block: assuming the new dest is us, whether to block the communication from the targets to the dest
    void spoofing_attack(
        vector<tuple<shared_ptr<net_access>, mac_addr, ip4_addr>> targets,
        ip4_addr dest_ip,
        mac_addr src_mac, ip4_addr src_ip,
        bool block);
}