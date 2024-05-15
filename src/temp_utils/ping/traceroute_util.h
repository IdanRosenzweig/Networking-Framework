#ifndef SERVERCLIENT_TRACEROUTE_UTIL_H
#define SERVERCLIENT_TRACEROUTE_UTIL_H

#include "ping_util.h"

class traceroute_util : msg_receiver {
    msg_gateway * gateway;
    ip4_protocol ip_client;
    icmp_protocol icmp_client;

    circular_buffer<received_msg> raw_icmp;
    void handle_received_event(received_msg &event) override {
        raw_icmp.push_back(event);
    }

public:
    traceroute_util(msg_gateway * gw = nullptr);

    next_choice<ip4_addr> dest_ip;

    void trace_to_destination();

};


#endif //SERVERCLIENT_TRACEROUTE_UTIL_H
