#ifndef SERVERCLIENT_BASIC_SNIFFER_H
#define SERVERCLIENT_BASIC_SNIFFER_H

#include "../receiving/multi_receiver.h"
#include "../receiving/received_msg.h"

enum packet_type {
    OUTGOING,
    INCOMING
};

struct sniffed_packet {
    packet_type type;
    received_msg msg;
};

class basic_sniffer {
public:
    virtual void handle_outgoing_packet(received_msg& msg) = 0;
    virtual void handle_incoming_packet(received_msg& msg) = 0;
};

#endif //SERVERCLIENT_BASIC_SNIFFER_H
