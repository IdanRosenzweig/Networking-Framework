#ifndef NETWORKING_IFACE_ACCESS_POINT_H
#define NETWORKING_IFACE_ACCESS_POINT_H

#include "../../abstract/utils/network_atomic_queue.h"
#include "../../abstract/receiving/recv_forwarder.h"
#include "../../abstract/sending/basic_send_medium.h"

#include <pcap/pcap.h>
#include <string>
#include <thread>
using namespace std;

void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);
void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

// raw access point for a linux interface
class iface_access_point {
    // listening for outgoing traffic
    pcap_t *traffic_out;
    std::thread worker_out;
    network_atomic_queue<recv_msg_t> outgoing_network_queue;
    friend void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

    // listening for incoming traffic
    pcap_t *traffic_in;
    std::thread worker_in;
    network_atomic_queue<recv_msg_t> incoming_network_queue;
    friend void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

    // injecting buff out
    int fd;

public:

    const string& iface_name;
    iface_access_point(const string& iface);

    ~iface_access_point();

    // use for listening to traffic coming out of the interface
    recv_forwarder<recv_msg_t> outgoing_traffic;

    // use for listening to traffic coming into the interface
    recv_forwarder<recv_msg_t> incoming_traffic;

    // use to send message out
    int send_out(void* ptr, int cnt);

};


#endif //NETWORKING_IFACE_ACCESS_POINT_H
