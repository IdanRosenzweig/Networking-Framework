#ifndef SERVERCLIENT_ARP_CONN_CLIENT_H
#define SERVERCLIENT_ARP_CONN_CLIENT_H

#include <string>
#include <vector>
#include <utility>

#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/circular_buffer.h"
#include "../ether/mac_addr.h"
#include "../ether/ethernet_protocol.h"

class arp_conn_client : public msg_receiver {
public:
    ethernet_protocol* gateway;


    circular_buffer<received_msg> message_q;

    void handle_received_event(received_msg msg) override;


    // find mac addr associated with priv_ip, send result to source
    mac_addr search_for_mac_addr(std::string priv_ip,
                                 mac_addr source_mac, ip4_addr source_ip);

    // spoof the victim_devices into thinking that the target_ip addr
    // is associated with the source_mac addr (empty victim list means broadcast)
    void spoof_as_device(std::string target_ip, mac_addr source_mac, std::vector<std::pair<mac_addr, std::string>>& victim_devices);

};

#endif //SERVERCLIENT_ARP_CONN_CLIENT_H
