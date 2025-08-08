#pragma once

#include "src/abstract/sending/send_medium.h"
#include "src/abstract/receiving/recv_listener.h"
#include "src/abstract/receiving/async_recv_listener.h"

#include "src/abstract/receiving/recv_queue.h"

#include <pcap/pcap.h>

#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
using namespace std;

void my_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

class macos_iface {
private:
    // catching incoming traffic to the iface
    pcap_t* traffic;
    thread worker_traffic;
    friend void my_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes);

    // injecting outgoing data to the iface
    shared_ptr<send_medium_bytes> send_access;
    int fd;

    // recv access for incoming data (async queue)
    shared_ptr<async_recv_listener<vector<uint8_t>>> async_recv;

    // sniff
    shared_ptr<recv_listener_bytes> sniff;

public:
    string const& iface_name;
    macos_iface(string const& iface);

    ~macos_iface();

public:
    // send access for outgoing data
    shared_ptr<send_medium_bytes> outgoing_get_send() {
        return send_access;
    }

    // async recv access for incoming data
    void incoming_set_recv(shared_ptr<recv_listener_bytes> const& recv) {
        async_recv->set_recv_listener(recv);
    }

    // async sniff for incoming data
    void set_sniffing(shared_ptr<recv_listener_bytes> const& sniff) {
        this->sniff = sniff;
    }
};