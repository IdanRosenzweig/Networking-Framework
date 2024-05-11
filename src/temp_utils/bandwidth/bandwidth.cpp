#include "bandwidth.h"

void bandwidth::my_sniff::handle_outgoing_packet(received_msg &msg) {
    master->bytes_out_cnt += msg.sz;
}

void bandwidth::my_sniff::handle_incoming_packet(received_msg &msg) {
    master->bytes_in_cnt += msg.sz;
}

bandwidth::bandwidth(multi_sniffer *conn) : conn(conn), sniff(this) {
    conn->add_sniffer(&sniff);
}


