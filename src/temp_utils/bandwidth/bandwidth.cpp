#include "bandwidth.h"

void bandwidth::outgoing_sniff::handle_received_event(received_msg &&event) {
    master->bytes_out_cnt += event.data.size();
}

void bandwidth::incoming_sniff::handle_received_event(received_msg &&event) {
    master->bytes_in_cnt += event.data.size();
}

bandwidth::bandwidth(struct sniffer *_sniffer) : sniffer(_sniffer), outgoing_sniff(this), incoming_sniff(this) {
    sniffer->outgoing.sniffers.push_back(&outgoing_sniff);
    sniffer->incoming.sniffers.push_back(&incoming_sniff);
}


