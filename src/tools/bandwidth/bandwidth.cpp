#include "bandwidth.h"

void bandwidth::outgoing_sniff::handle_callback(recv_msg_t &&data) {
    master->bytes_out_cnt += data.buff.size();
}

void bandwidth::incoming_sniff::handle_callback(recv_msg_t &&data) {
    master->bytes_in_cnt += data.buff.size();
}

bandwidth::bandwidth(struct sniffer *_sniffer) : sniffer(_sniffer), outgoing_sniff(this), incoming_sniff(this) {
    sniffer->outgoing.sniffers.push_back(&outgoing_sniff);
    sniffer->incoming.sniffers.push_back(&incoming_sniff);
}


