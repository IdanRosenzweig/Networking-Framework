#ifndef SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
#define SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H

#include <netpacket/packet.h>
#include <thread>
#include <pcap/pcap.h>

#include "../../abstract/connection/basic_gateway.h"
#include "data_link_sniffer.h"

class data_link_layer_gateway : public basic_gateway {
private:
    int fd;

    data_link_sniffer sniffer;

public:

    data_link_layer_gateway(const string& interface);

    ~data_link_layer_gateway();

    int send_data(send_msg msg) override;

};

#endif //SERVERCLIENT_DATA_LINK_LAYER_GATEWAY_H
