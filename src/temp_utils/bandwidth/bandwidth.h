#ifndef NETWORKING_BANDWIDTH_H
#define NETWORKING_BANDWIDTH_H

#include "../../abstract/connection/connection.h"
#include "../../abstract/sniffer/sniffer.h"

class bandwidth {
    struct sniffer* sniffer;

    class outgoing_sniff : public basic_sniff_handler {
        bandwidth* master;
    public:
        explicit outgoing_sniff(bandwidth *master) : master(master) {}

        void handle_received_event(received_msg &&event) override;

    };
    outgoing_sniff outgoing_sniff;

    class incoming_sniff : public basic_sniff_handler {
        bandwidth* master;
    public:
        explicit incoming_sniff(bandwidth *master) : master(master) {}

        void handle_received_event(received_msg &&event) override;
    };
    incoming_sniff incoming_sniff;

public:
    explicit bandwidth(struct sniffer *sniffer);

    uint64_t bytes_out_cnt = 0;
    uint64_t bytes_in_cnt = 0;

};


#endif //NETWORKING_BANDWIDTH_H
