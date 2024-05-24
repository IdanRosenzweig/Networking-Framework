#ifndef SERVERCLIENT_NET_ARP_H
#define SERVERCLIENT_NET_ARP_H

#include <string>
#include <vector>
#include <utility>

#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/receiving/msg/msg_receiver.h"
#include "../../abstract/receiving/receive_queue.h"
#include "../../abstract/gateway/gateway.h"
#include "../../abstract/utils/circular_buffer.h"

#include "../../linux/interface_gateway.h"

#include "../../protocols/ether/mac_addr.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_addr.h"

#include <linux/if_ether.h>

class net_arp : private receive_queue<received_msg> {

    // send
    gateway *data_link_layer_gateway;
    ethernet_protocol ether_client;

public:

    net_arp(gateway *gw) : data_link_layer_gateway(gw) {
        ether_client.gateway = data_link_layer_gateway;
//        ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
//        ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);
//        ether_client.next_source_addr.set_next_choice(my_mac);

        // for receiving
        data_link_layer_gateway->add_listener(&ether_client);
        ether_client.protocol_handlers.assign_to_key(htons(ETH_P_ARP), this);
    }

    ~net_arp() {
        data_link_layer_gateway->remove_listener(&ether_client);
    }

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


};

#endif //SERVERCLIENT_NET_ARP_H
