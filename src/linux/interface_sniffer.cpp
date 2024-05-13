#include "interface_sniffer.h"
#include <unistd.h>

void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    interface_sniffer *sniffer = reinterpret_cast<interface_sniffer *>(user);

    int data_sz = pkthdr->len;
    if (data_sz <= 0) return;

    uint8_t *alloc_msg = new uint8_t[data_sz];
    memcpy(alloc_msg, bytes, data_sz);

    received_msg msg;
    msg.data = unique_ptr<uint8_t>(alloc_msg);
    msg.sz = data_sz;
    msg.curr_offset = 0;
    sniffer->handle_incoming_packet(msg);
}

void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    interface_sniffer *sniffer = reinterpret_cast<interface_sniffer *>(user);

    int data_sz = pkthdr->len;
    if (data_sz <= 0) return;

    uint8_t *alloc_msg = new uint8_t[data_sz];
    memcpy(alloc_msg, bytes, data_sz);

    received_msg msg;
    msg.data = unique_ptr<uint8_t>(alloc_msg);
    msg.sz = data_sz;
    msg.curr_offset = 0;
    sniffer->handle_outgoing_packet(msg);
}

interface_sniffer::interface_sniffer(const string& interface) {

    char errbuf[PCAP_ERRBUF_SIZE];

    // traffic in
    {
        traffic_in = pcap_open_live(interface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic_in == nullptr) {
            cerr << "can't open interface: " << interface << ", err: " << errbuf << endl;
            throw;
        }

        char *filter_exp = "inbound";

        struct bpf_program fp;
        if (pcap_compile(traffic_in, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
            return;
        }

        if (pcap_setfilter(traffic_in, &fp) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
            return;
        }

        worker_in = std::thread([this]() {
            pcap_loop(traffic_in, -1, handler_in, reinterpret_cast<u_char *>(this));
        });
    }


    // traffic out
    {
        traffic_out = pcap_open_live(interface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic_out == nullptr) {
            cerr << "can't open interface: " << interface << ", err: " << errbuf << endl;
            throw;
        }

        char *filter_exp = "outbound";

        struct bpf_program fp;
        if (pcap_compile(traffic_out, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
            return;
        }

        if (pcap_setfilter(traffic_out, &fp) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
            return;
        }

        worker_out = std::thread([this]() {
            pcap_loop(traffic_out, -1, handler_out, reinterpret_cast<u_char *>(this));
        });
    }

    sleep(2); // ensure the pcap_loop has started
}

interface_sniffer::~interface_sniffer() {
    worker_in.detach();
    worker_out.detach();
}