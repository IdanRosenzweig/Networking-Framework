#pragma once

#include "src/abstract/sending/basic_send_medium.h"
#include "src/abstract/receiving/basic_recv_listener.h"
#include "src/abstract/receiving/async_recv_listener.h"

#include "src/abstract/receiving/recv_queue.h"

#include <pcap/pcap.h>

#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
using namespace std;

void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);
void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

// raw access point for a linux interface
// class iface_access_point {
//     // listening for outgoing traffic
//     pcap_t *traffic_out;
//     std::thread worker_out;
//     network_atomic_queue<recv_msg_t> outgoing_network_queue;
//     friend void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

//     // listening for incoming traffic
//     pcap_t *traffic_in;
//     std::thread worker_in;
//     network_atomic_queue<recv_msg_t> incoming_network_queue;
//     friend void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

//     // injecting buff out
//     int fd;

// public:

//     const string& iface_name;
//     iface_access_point(const string& iface);

//     ~iface_access_point();

//     // use for listening to traffic coming out of the interface
//     recv_forwarder<recv_msg_t> outgoing_traffic;

//     // use for listening to traffic coming into the interface
//     recv_forwarder<recv_msg_t> incoming_traffic;

//     // use to send message out
//     int send_out(void* ptr, int cnt);

// };


class linux_iface {
private:
    // catching outgoing traffic from the iface
    pcap_t* traffic_out;
    thread worker_traffic_out;
    // network_atomic_queue<recv_msg_t> outgoing_network_queue;
    friend void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

    // catching incoming traffic to the iface
    pcap_t* traffic_in;
    thread worker_traffic_in;
    // network_atomic_queue<recv_msg_t> incoming_network_queue;
    friend void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

    // injecting outgoing data to the iface
    shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
    int fd;

    // recv queue
    shared_ptr<async_recv_listener<vector<uint8_t>>> async_recv;

public:
    string const& iface_name;
    linux_iface(string const& iface);

    ~linux_iface();

    shared_ptr<basic_send_medium<vector<uint8_t>>> get_send_access() {
        return send_access;
    }

    /* recv access, can be set from outside in order to receive from the iface */
// private:
//     shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;

public:
    void set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) {
        async_recv->set_recv_listener(recv);
        // recv_access = recv;
    }

};