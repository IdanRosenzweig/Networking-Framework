#include "data_link_layer_gateway.h"

#include <cstring>
#include <iostream>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;

void recv_packet(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    data_link_layer_gateway *gateway = reinterpret_cast<data_link_layer_gateway *>(user);

    int data_sz = pkthdr->len;

    if (data_sz <= 0) return;

    uint8_t *alloc_msg = new uint8_t[data_sz];
    memcpy(alloc_msg, bytes, data_sz);

    gateway->handle_received_msg(msg_protocol_stack{{unique_ptr<uint8_t>(alloc_msg), data_sz},
                                                    {}, 0});

}

data_link_layer_gateway::data_link_layer_gateway() {
    char *dev = "enp0s3";

    // open fd
    fd = socket(
            AF_PACKET,
            SOCK_RAW,
            htons(ETH_P_ALL)
    );
    if (fd == -1) {
        cerr << "data_link_layer_fd err" << endl;
        printf("errno: %d\n", errno);
        throw;
    }

    // get if_req
    struct ifreq if_idx;
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, dev, IFNAMSIZ - 1);

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP)); // todo change to ALL?
    if (ioctl(temp_fd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");
    close(temp_fd);

    // set ll socket data
    dest_addr.sll_ifindex = if_idx.ifr_ifindex;


    // setup sniffer
    char errbuf[PCAP_ERRBUF_SIZE]; // Error string
    traffic_in = pcap_open_live(dev, BUFSIZ, 1, 10, errbuf);
    if (traffic_in == nullptr) {
        cerr << "can't open interface: " << dev << ", err: " << errbuf << endl;
        throw;
    }

    char *filter_exp;
    if (true) filter_exp = "inbound";
    else filter_exp = "outbound";

    struct bpf_program fp;
    if (pcap_compile(traffic_in, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
        fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
        return;
    }

    if (pcap_setfilter(traffic_in, &fp) == PCAP_ERROR) {
        fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
        return;
    }

    worker = std::thread([this]() {
        pcap_loop(traffic_in, -1, recv_packet, reinterpret_cast<u_char *>(this));
    });

    sleep(2); // ensure the pcap_loop has started
}

data_link_layer_gateway::~data_link_layer_gateway() {
    close(fd);
    worker.detach();
}

int data_link_layer_gateway::send_data(void *buff, int count) {
    return sendto(fd,
                  buff, count,
                  0,
                  reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));
}