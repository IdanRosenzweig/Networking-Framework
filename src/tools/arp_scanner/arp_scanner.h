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

struct arp_scanner {
private:
    shared_ptr<recv_queue<shared_ptr<arp_header>>> queue;

    shared_ptr<arp_protocol> arp;

public:
    arp_scanner();

    void set_arp(shared_ptr<arp_protocol> const& _arp);

    // find mac address associated with searched_ip
    mac_addr search_for_mac_addr(ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip);

    // // scan the entire subnet for its ip addresses and their associated mac addresses
    // vector<pair<ip4_addr, mac_addr>> scan_entire_subnet(ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip, std::chrono::milliseconds delay);


    // // spoof the victims_ip devices into thinking that the target_ip address
    // // is associated with the source (is_empty victim list means broadcast)
    // void spoof_as_device(const std::vector<ip4_addr> &victims_ip, ip4_addr target_ip,
    //                      mac_addr src_mac, ip4_addr src_ip);

    // // sniff/block the traffic going from victim_devices into dest, possible also in the opposite direction.
    // // if victim_devices, sends to broadcast
    // void intercept_device_traffic(const vector<ip4_addr> &victim_devices, ip4_addr dest, bool block, mac_addr src_mac,
    //                               ip4_addr src_ip);


    // // announce a new mac address associated with the src ip
    // void announce_new_mac(ip4_addr src_ip, mac_addr new_mac);
};