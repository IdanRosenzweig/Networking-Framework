#ifndef NETWORKING_NET_ARP_H
#define NETWORKING_NET_ARP_H

#include <string>
#include <vector>

#include "../../abstract/sending/msg/send_msg_t.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"
#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/gateway/gateway.h"
#include "../../abstract/utils/circular_queue.h"

#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_addr.h"

class net_arp {
    gateway *data_link_layer_gateway;

    ethernet_protocol ether_client;
    recv_queue<recv_msg_t> arp_recv_q;

public:

    net_arp(gateway *gw);

    ~net_arp();

    // find mac address associated with searched_ip
    mac_addr search_for_mac_addr(ip4_addr searched_ip, mac_addr src_mac, ip4_addr src_ip);

    // scan the entire subnet for its ip addresses and their associated mac addresses
    vector<pair<ip4_addr, mac_addr>> scan_entire_subnet(ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip);


    // spoof the victims_ip devices into thinking that the target_ip address
    // is associated with the source (is_empty victim list means broadcast)
    void spoof_as_device(const std::vector<ip4_addr> &victims_ip, ip4_addr target_ip,
                         mac_addr src_mac, ip4_addr src_ip);

    // sniff/block the traffic going from victim_devices into dest, possible also in the opposite direction.
    // if victim_devices, sends to broadcast
    void intercept_device_traffic(const vector<ip4_addr> &victim_devices, ip4_addr dest, bool block, mac_addr src_mac,
                                  ip4_addr src_ip);


    // announce a new mac address associated with the src ip
    void announce_new_mac(ip4_addr src_ip, mac_addr new_mac);

};

#endif //NETWORKING_NET_ARP_H
