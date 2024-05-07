#ifndef SERVERCLIENT_ARP_PROTOCOL_H
#define SERVERCLIENT_ARP_PROTOCOL_H

#include <string>
#include <vector>
#include <utility>

#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/utils/circular_buffer.h"
#include "../ether/mac_addr.h"
#include "../ether/ethernet_protocol.h"

class arp_protocol :public msg_receiver {
public:

    ethernet_protocol* gateway;


    // recv
    circular_buffer<received_msg> message_q;
    basic_receiver* default_handler = nullptr;

    void handle_received_event(received_msg& msg) override;


    // find mac octets associated with priv_ip, send result to source
    mac_addr search_for_mac_addr(std::string priv_ip,
                                 mac_addr source_mac, ip4_addr source_ip);

    // spoof the victim_devices into thinking that the target_ip octets
    // is associated with the source_mac octets (empty victim list means broadcast)
    void spoof_as_device(std::string target_ip, mac_addr source_mac, std::vector<std::pair<mac_addr, std::string>>& victim_devices);

};

#endif //SERVERCLIENT_ARP_PROTOCOL_H
