#ifndef SERVERCLIENT_DATA_LINK_SNIFFER_H
#define SERVERCLIENT_DATA_LINK_SNIFFER_H

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <thread>

#include <netpacket/packet.h>
#include <linux/if.h>
#include <pcap/pcap.h>
#include "../../abstract/receiving/listenable.h"
#include "../../abstract/receiving/received_msg.h"

class data_link_sniffer : public listenable<received_msg> {
    pcap_t *traffic;
    std::thread worker;

public:
    data_link_sniffer(bool in);

    ~data_link_sniffer();

};


#endif //SERVERCLIENT_DATA_LINK_SNIFFER_H
