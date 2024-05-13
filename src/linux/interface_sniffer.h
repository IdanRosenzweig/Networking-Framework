#ifndef SERVERCLIENT_INTERFACE_SNIFFER_H
#define SERVERCLIENT_INTERFACE_SNIFFER_H

#include "../abstract/connection/multi_sniffer.h"

#include <pcap/pcap.h>
#include <string>
#include <thread>
using namespace std;

class interface_sniffer : public multi_sniffer {
    pcap_t *traffic_in;
    std::thread worker_in;

    pcap_t *traffic_out;
    std::thread worker_out;

public:
    interface_sniffer(const string& interface);

    ~interface_sniffer();

};


#endif //SERVERCLIENT_INTERFACE_SNIFFER_H
