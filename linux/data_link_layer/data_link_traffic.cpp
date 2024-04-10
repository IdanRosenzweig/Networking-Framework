#include "data_link_traffic.h"

#include <iostream>
#include <cstring>

using namespace std;

data_link_traffic::data_link_traffic(bool in) {
    char* dev = "enp0s3";

    char errbuf[PCAP_ERRBUF_SIZE]; // Error string
    traffic = pcap_open_live(dev, BUFSIZ, 1, 100, errbuf);
    if (traffic == nullptr) {
        cerr << "can't open interface: " << dev << ", err: " << errbuf << endl;
        throw;
    }

    char *filter_exp;
    if (in) filter_exp = "inbound";
    else filter_exp = "outbound";

    struct bpf_program fp;
    if (pcap_compile(traffic, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(traffic));
        return;
    }

    if (pcap_setfilter(traffic, &fp) == PCAP_ERROR) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(traffic));
        return;
    }

}

data_link_traffic::~data_link_traffic() {
    pcap_close(traffic);
}

int data_link_traffic::sniff_next_msg(void *buff, int count) {
    char *packet_buff;
    struct pcap_pkthdr * pcap_header;

    while (true) {
        int res = pcap_next_ex(traffic, &pcap_header, (const u_char **) (&packet_buff));
        if (res == PCAP_ERROR)
            continue;

        int copy_cnt = std::min(count, (int) pcap_header->len);
        memcpy(buff, packet_buff, copy_cnt);
        return copy_cnt;
    }
}
