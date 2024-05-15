#include "interface_gateway.h"

#include <cstring>
#include <iostream>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <linux/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;


interface_gateway::interface_gateway(const string &interface)
//        : sniffer(interface, true)
        : if_sniffer(interface)
    {

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
    memset(&outputServer, 0, sizeof(struct sockaddr_ll));

    outputServer.sll_family = AF_PACKET;
    outputServer.sll_protocol = htons(ETH_P_ALL);
    outputServer.sll_ifindex = if_idx.ifr_ifindex;
    outputServer.sll_halen = ETH_ALEN;

    if (bind(fd, (struct sockaddr *) &outputServer, sizeof(outputServer)) == -1)
        perror("bind");

    // set ll socket msg
//    dest_addr.sll_ifindex = if_idx.ifr_ifindex;

//    sniffer.add_listener(this);
    if_sniffer.add_sniffer(this);

}

interface_gateway::~interface_gateway() {
    close(fd);
//    worker.detach();
}

int interface_gateway::send_data(send_msg& msg) {
//    return sendto(fd,
//                msg.buff, msg.count,
//                0,
//                reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));
    return send(fd,
                  msg.buff, msg.count,
                  0);
}

void interface_gateway::handle_outgoing_packet(received_msg &msg) {
    // nothing
}

void interface_gateway::handle_incoming_packet(received_msg &msg) {
    this->handle_received_event(msg);
}
