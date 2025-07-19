#pragma once

#include <memory>
#include <vector>
#include <utility>
using namespace std;

#include "src/protocols/ether/ethernet2_protocol.h"
#include "src/protocols/arp/arp_protocol.h"

#include "src/protocols/ether/mac_addr.h"
#include "src/protocols/ip4/ip4_addr.h"

#include "src/abstract/receiving/recv_queue.h"

namespace arp_scanner {
    mac_addr search_for_mac_addr(shared_ptr<network_access_bytes> const& broadcast_access, ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip);

    vector<pair<ip4_addr, mac_addr>> scan_entire_subnet(shared_ptr<network_access_bytes> const& broadcast_access, ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip);

    void announce_new_pair(shared_ptr<network_access_bytes> const& broadcast_access, ip4_addr src_ip, mac_addr new_mac);
}

// struct arp_scanner {
// private:
//     shared_ptr<recv_queue<shared_ptr<arp_header>>> queue;

//     shared_ptr<arp_protocol> arp;

// public:
//     arp_scanner();

//     void set_arp(shared_ptr<arp_protocol> const& _arp);

//     // find mac address associated with searched_ip
//     mac_addr search_for_mac_addr(ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip);

//     // scan all of the ip addrs under a subnet for their associated mac addrs
//     vector<pair<ip4_addr, mac_addr>> scan_entire_subnet(ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip);

//     // sniff/block the traffic going from victim_devices into dest, possible also in the opposite direction.
//     // if victim_devices, sends to broadcast
//     // void intercept_device_traffic(const vector<ip4_addr> &victim_devices, ip4_addr dest, bool block, mac_addr src_mac,
//     //                               ip4_addr src_ip);

//     // announce a new ip4 addr <-> mac addr association
//     void announce_new_pair(ip4_addr src_ip, mac_addr new_mac);
// };