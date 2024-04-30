#include "data_link_sniffer.h"

#include <iostream>
#include <cstring>

using namespace std;

void handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    data_link_sniffer *sniffer = reinterpret_cast<data_link_sniffer *>(user);

    int data_sz = pkthdr->len;
    if (data_sz <= 0) return;

    uint8_t *alloc_msg = new uint8_t[data_sz];
    memcpy(alloc_msg, bytes, data_sz);

    received_msg msg;
    msg.data = unique_ptr<uint8_t>(alloc_msg);
    msg.sz = data_sz;
    msg.curr_offset = 0;
    sniffer->handle_received_event(msg);
}

data_link_sniffer::data_link_sniffer(bool in) {
    char* dev = "enp0s3";

    char errbuf[PCAP_ERRBUF_SIZE]; // Error string
    traffic = pcap_open_live(dev, BUFSIZ, 1, 10, errbuf);
    if (traffic == nullptr) {
        cerr << "can't open interface: " << dev << ", err: " << errbuf << endl;
        throw;
    }

    char *filter_exp;
    if (in) filter_exp = "inbound";
    else filter_exp = "outbound";

    struct bpf_program fp;
    if (pcap_compile(traffic, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
        fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic));
        return;
    }

    if (pcap_setfilter(traffic, &fp) == PCAP_ERROR) {
        fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic));
        return;
    }

    worker = std::thread([this]() {
        pcap_loop(traffic, -1, handler, reinterpret_cast<u_char *>(this));
    });

    sleep(2); // ensure the pcap_loop has started

}

data_link_sniffer::~data_link_sniffer() {
    pcap_close(traffic);
    worker.detach();
}
