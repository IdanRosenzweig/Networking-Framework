#include "data_link_layer_gateway.h"

#include <cstring>
#include <iostream>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <linux/socket.h>
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

data_link_layer_gateway::data_link_layer_gateway(const string& interface) : sniffer(interface, true) {

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
    strncpy(if_idx.ifr_name, interface.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");

    struct sockaddr_ll outputServer;
    memset(&outputServer, 0, sizeof (struct sockaddr_ll));

    outputServer.sll_family = AF_PACKET;
    outputServer.sll_protocol = htons(ETH_P_ALL);
    outputServer.sll_ifindex = if_idx.ifr_ifindex;
    outputServer.sll_halen = ETH_ALEN;  // 6

    // is this needed for writing RAW ethernet out, even as tcpSession???
    if (bind(fd, (struct sockaddr *) & outputServer, sizeof (outputServer)) == -1)
        perror("bind");


//    // set ll socket msg
//    dest_addr.sll_ifindex = if_idx.ifr_ifindex;

    sniffer.add_listener(this);
}

data_link_layer_gateway::~data_link_layer_gateway() {
    close(fd);
//    worker.detach();
}

int data_link_layer_gateway::send_data(send_msg msg) {
    return send(fd,
                  msg.buff, msg.count,
                  0);
}