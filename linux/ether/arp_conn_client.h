#ifndef SERVERCLIENT_ARP_CONN_CLIENT_H
#define SERVERCLIENT_ARP_CONN_CLIENT_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
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

    // spoof device to think that the private ip is associated with this mac addr
    void spoof(const std::string& spoofed_mac, const std::string& ip,
               const std::string& device_mac);
};

#endif //SERVERCLIENT_ARP_CONN_CLIENT_H
