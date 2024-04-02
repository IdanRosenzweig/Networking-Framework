#ifndef SERVERCLIENT_ARP_CONN_CLIENT_H
#define SERVERCLIENT_ARP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"
#include "ethernet_conn_client.h"

class arp_conn_client : public basic_cl_client {
public:
    arp_conn_client();

    void init() override;

    void finish() override;

//    int send_encapsulated_data(void *buff, int count) override;
//
//    int recv_encapsulated_data(void *buff, int count) override;

    ethernet_conn_client* ether_client;

    mac_addr search_for_device(std::string priv_ip);

    // spoof the victim devices into thinking that the private ip addr
    // is associated with our mac addr (empty victim list means broadcast)
    void spoof_as_device(std::string device, std::vector<std::pair<mac_addr, std::string>>& victim_devices);
};

#endif //SERVERCLIENT_ARP_CONN_CLIENT_H
