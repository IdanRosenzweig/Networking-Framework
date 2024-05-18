#include "bandwidth.h"

void bandwidth::my_sniff::handle_outgoing_packet(received_msg &msg) {
    master->bytes_out_cnt += msg.data.size();
}

void bandwidth::my_sniff::handle_incoming_packet(received_msg &msg) {
    master->bytes_in_cnt += msg.data.size();
}

bandwidth::bandwidth(multi_msg_sniffer *sniffer) : conn(sniffer), my_sniff(this) {
    conn->add_sniffer(&my_sniff);
}


