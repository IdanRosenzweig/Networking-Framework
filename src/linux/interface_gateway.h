#ifndef SERVERCLIENT_INTERFACE_GATEWAY_H
#define SERVERCLIENT_INTERFACE_GATEWAY_H

#include <netpacket/packet.h>
#include <thread>
#include <pcap/pcap.h>

#include "../abstract/gateway/basic_gateway.h"
#include "interface_sniffer.h"

class interface_gateway;

class interface_gateway : private basic_sniffer, public basic_gateway {
private:
    int fd;
    struct sockaddr_ll dest_addr; // used only for hardware interface

//    data_link_sniffer sniffer;
    interface_sniffer if_sniffer;

public:
    interface_gateway(const string& interface);

    ~interface_gateway();

    int send_data(send_msg msg) override;

private:
    void handle_outgoing_packet(received_msg &msg) override;

    void handle_incoming_packet(received_msg &msg) override;

};

#endif //SERVERCLIENT_INTERFACE_GATEWAY_H
