#ifndef SERVERCLIENT_ARP_SCANNER_H
#define SERVERCLIENT_ARP_SCANNER_H

#include <string>
#include <vector>
#include <utility>

#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/utils/circular_buffer.h"

#include "../../linux/data_link_layer/data_link_layer_gateway.h"

#include "../../protocols/ether/mac_addr.h"
#include "../../protocols/ether/ethernet_protocol.h"
#include "../../protocols/ip4/ip4_addr.h"

#include <linux/if_ether.h>

class arp_scanner : private recv_queue<received_msg> {

    // send
    data_link_layer_gateway dataLinkLayerGateway;
    ethernet_protocol ether_client;

public:

    arp_scanner() : dataLinkLayerGateway("enp0s3") {
        ether_client.gateway = &dataLinkLayerGateway;
//        ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
//        ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);
//        ether_client.next_source_addr.set_next_choice(my_mac);

        // for receiving
        dataLinkLayerGateway.add_listener(&ether_client);
        ether_client.protocol_handlers.assign_to_key(htons(ETH_P_ARP), this);
    }

    // find mac address associated with searched_ip
    mac_addr search_for_mac_addr(ip4_addr searched_ip);

    // scan the entire subnet for its ip addresses and their associated mac addresses
    vector<pair<ip4_addr, mac_addr>> scan_entire_subnet(ip4_subnet_mask mask);

    // spoof the victim_devices into thinking that the target_ip octets
    // is associated with the source_mac octets (empty victim list means broadcast)
//    void spoof_as_device(std::string target_ip, mac_addr source_mac, std::vector<std::pair<mac_addr, std::string>>& victim_devices);

};

#endif //SERVERCLIENT_ARP_SCANNER_H
