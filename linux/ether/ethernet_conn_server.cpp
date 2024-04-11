#include <cstring>
#include "ethernet_conn_server.h"
#include <iostream>
#include <netinet/ip.h>
#include <cstring>
#include <linux/if.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include "mac_addr.h"
#include "ethernet_conn_client.h"
#include "../udp/udp_header.h"
using namespace std;

ethernet_conn_server::ethernet_conn_server() {
    char *INTERFACE = "enp0s3";

    my_mac = get_my_mac_address(INTERFACE);

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));

//    /* Get the index of the INTERFACE to send on */
//    memset(&if_idx, 0, sizeof(struct ifreq));
//    strncpy(if_idx.ifr_name, INTERFACE, IFNAMSIZ - 1);
//    if (ioctl(temp_fd, SIOCGIFINDEX, &if_idx) < 0)
//        perror("SIOCGIFINDEX");
//
//    dest_addr.sll_ifindex = if_idx.ifr_ifindex;


    // Get the private ip address of our device
    memset(&my_priv_ip, 0, sizeof(struct ifreq));
    strncpy(my_priv_ip.ifr_name, INTERFACE, IFNAMSIZ - 1);
    if (ioctl(temp_fd, SIOCGIFADDR, &my_priv_ip) < 0)
        perror("SIOCGIFADDR");

    close(temp_fd);

    worker = std::thread([this]() {
#define BUFF_LEN 1024
        char buff[BUFF_LEN];
        while (true) {
            memset(buff, '\x00', BUFF_LEN);

            int sz = gateway.recv_raw(buff, BUFF_LEN);
            if (sz <= 0) continue;

            struct ether_header *eth_header = (struct ether_header *) buff;

            char * data = buff + sizeof(struct ether_header);
            int data_sz = sz - sizeof(struct ether_header);

            uint8_t* alloc_msg = new uint8_t[data_sz];
            memcpy(alloc_msg, data, data_sz);

            protocolQueue.prots[eth_header->ether_type].push_back(
                    {std::unique_ptr<uint8_t>(alloc_msg), data_sz}
            );

        }
    });

    // wait to ensure the thread and the sniffer has started
    sleep(2);
}

ethernet_conn_server::~ethernet_conn_server() {
    worker.detach();
}

int ethernet_conn_server::recv_next_msg( void *data, int count) {
    while (protocolQueue.prots[getNextProt()].empty()) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
    }

    message next_msg = std::move(
            protocolQueue.prots[getNextProt()].front()
    );
    protocolQueue.prots[getNextProt()].pop_front();

//    int sub_prot = ((struct iphdr*) next_msg.data.get())->protocol;
//    switch (sub_prot) {
//        case IPPROTO_UDP: {
//            struct udp_header* udp_h = (struct udp_header*) (next_msg.data.get() + sizeof(struct iphdr));
//            cout << "udp " << ntohs(udp_h->dest_port) << endl;
//            cout << (char*) (next_msg.data.get() + sizeof(struct iphdr) + sizeof(struct udp_header)) << endl;
//            break;
//        }
//        case IPPROTO_TCP: {
//            cout << "tcp" << endl;
//            break;
//        }
//        case IPPROTO_ICMP: {
//            cout << "icmp" << endl;
//            break;
//        }
//    }

    int copy_cnt = std::min(count, next_msg.sz);
    memcpy(data, next_msg.data.get(), copy_cnt);
    return copy_cnt;
}

int ethernet_conn_server::send_next_msg( void *data, int count) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, 0, BUFF_LEN);

    // setup ethernet
    struct ether_header *eth_header = (struct ether_header *) buff;

    // dest mac addr
    memcpy(eth_header->ether_dhost, &dest_device, ETH_ALEN);
//    memset(eth_header->ether_dhost, 0xff, ETH_ALEN); // send to broadcast

// source (my mac)
    memcpy(eth_header->ether_shost, &my_mac, ETH_ALEN); // my mac

    // ethertype
    eth_header->ether_type = getNextProt();


    char* frame_data = buff + sizeof(struct ether_header);
    memcpy(frame_data, data, count);

    return gateway.send_raw(buff, sizeof(struct ether_header) + count);

}

void ethernet_conn_server::change_dest_mac(mac_addr mac) {
    dest_device = mac;
}