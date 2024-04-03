#ifndef SERVERCLIENT_DATA_LINK_TRAFFIC_H
#define SERVERCLIENT_DATA_LINK_TRAFFIC_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "../../abstract/basic_encapsulating_client.h"
#include "../../abstract/connectionless/basic_cl_client.h"
#include "../../abstract/basic_gateway.h"

#include <netpacket/packet.h>
#include <linux/if.h>
#include <pcap/pcap.h>
#include "../ip4/addit_data.h"
#include "../ether/mac_addr.h"

class data_link_traffic {
    pcap_t *traffic;

public:
    data_link_traffic(bool in);

    virtual ~data_link_traffic();

    int sniff_next_msg(void* buff, int count);
};


#endif //SERVERCLIENT_DATA_LINK_TRAFFIC_H
