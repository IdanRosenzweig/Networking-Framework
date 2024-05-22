#ifndef SERVERCLIENT_BANDWIDTH_H
#define SERVERCLIENT_BANDWIDTH_H

#include "../../abstract/connection/connection.h"
#include "../../abstract/sniffer/msg_sniffer.h"

class bandwidth {
    multi_msg_sniffer* conn;

    class my_sniff : public msg_sniffer {
        bandwidth* master;
    public:
        explicit my_sniff(bandwidth *master) : master(master) {}

        void handle_outgoing_packet(const received_msg &msg) override;

        void handle_incoming_packet(const received_msg &msg) override;
    };
    my_sniff my_sniff;


public:
    explicit bandwidth(multi_msg_sniffer *conn);

    uint64_t bytes_out_cnt = 0;
    uint64_t bytes_in_cnt = 0;

};


#endif //SERVERCLIENT_BANDWIDTH_H
