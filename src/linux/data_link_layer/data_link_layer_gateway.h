#ifndef SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
#define SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H

#include <netpacket/packet.h>
#include <thread>
#include <pcap/pcap.h>

#include "../../abstract/gateway/basic_gateway.h"
//#include "data_link_sniffer.h"
#include "interface_sniffer.h"
class data_link_layer_gateway : private basic_sniffer, public basic_gateway {
private:
    int fd;

//    data_link_sniffer sniffer;
    interface_sniffer if_sniffer;

public:

    data_link_layer_gateway(const string& interface);

    ~data_link_layer_gateway();

    int send_data(send_msg msg) override;

private:
    void handle_outgoing_packet(received_msg &msg) override;

    void handle_incoming_packet(received_msg &msg) override;

};

#endif //SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
