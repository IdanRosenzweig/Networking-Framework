#ifndef SERVERCLIENT_BASIC_SNIFFER_H
#define SERVERCLIENT_BASIC_SNIFFER_H

template <typename SEND_T, typename RECV_T>
class basic_sniffer {
public:
    virtual void handle_outgoing_packet(SEND_T& msg) = 0;
    virtual void handle_incoming_packet(RECV_T& msg) = 0;
};

#endif //SERVERCLIENT_BASIC_SNIFFER_H
