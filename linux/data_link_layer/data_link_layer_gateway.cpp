#include "data_link_layer_gateway.h"

#include <iostream>
#include <netinet/ip.h>
#include <cstring>
#include <linux/if.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include <thread>

using namespace std;


data_link_layer_gateway::data_link_layer_gateway() : traffic_in_sniff(true) {
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
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, dev, IFNAMSIZ - 1);

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP)); // todo change to ALL?
    if (ioctl(temp_fd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");
    close(temp_fd);


    // set ll socket data
    dest_addr.sll_ifindex = if_idx.ifr_ifindex;


}

data_link_layer_gateway::~data_link_layer_gateway() {
    close(fd);
}

int data_link_layer_gateway::send_data(void *buff, int count) {
    if (tunnel != nullptr) {
        return tunnel->send_data(buff, count);
    } else
        return sendto(fd,
                      buff, count,
                      0,
                      reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));
}

int data_link_layer_gateway::recv_data(void *buff, int count) {
    if (tunnel != nullptr) {
        return tunnel->recv_data(buff, count);
    } else {
        int res = traffic_in_sniff.sniff_next_msg(buff, count);
//    cout << "recived from raw sniffer: " << res << endl;
        return res;
    }
}
