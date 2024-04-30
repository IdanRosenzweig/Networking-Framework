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

    received_msg msg;
    msg.data = unique_ptr<uint8_t>(alloc_msg);
    msg.sz = data_sz;
    msg.curr_offset = 0;
    gateway->handle_received_event(msg);

}

data_link_layer_gateway::data_link_layer_gateway() : sniffer(true) {
    char *dev = "enp0s3";

    // open sd
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

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ioctl(temp_fd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");
    close(temp_fd);

    // set ll socket msg
    dest_addr.sll_ifindex = if_idx.ifr_ifindex;


    sniffer.add_listener(this);
}

data_link_layer_gateway::~data_link_layer_gateway() {
    close(fd);
//    worker.detach();
}

int data_link_layer_gateway::send_data(send_msg msg) {
    return sendto(fd,
                  msg.buff, msg.count,
                  0,
                  reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));
}