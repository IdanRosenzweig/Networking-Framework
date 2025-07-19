#include "arp_scanner.h"

#include "src/protocols/ether/ethertype.h"

#include <linux/if_arp.h> // todo remove this

#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdint>
using namespace std;


mac_addr arp_scanner::search_for_mac_addr(shared_ptr<network_access_bytes> const& broadcast_access, ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip) {
    // set arp over the broadcast access
    shared_ptr<arp_protocol> arp = make_shared<arp_protocol>();
    arp->set_net_access(broadcast_access);

    // listen to arp replies via a queue
    shared_ptr<recv_queue<shared_ptr<arp_header>>> queue = make_shared<recv_queue<shared_ptr<arp_header>>>();
    arp->listener_op[ARPOP_REPLY] = queue; // todo this value from my own list

    // arp header
    struct arp_header arp_header;
    arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
    arp_header.prot_type = ethertype::ip4;
    arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
    arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
    arp_header.op = ARPOP_REQUEST; // operation (request) // todo this value from my own list

    arp_header.sender_hard_addr = vector<uint8_t>(src_mac.octets, src_mac.octets + sizeof(src_mac.octets));
    arp_header.sender_prot_addr = vector<uint8_t>(src_ip.octets, src_ip.octets + sizeof(src_ip.octets));

    mac_addr empty_mac{{0, 0, 0, 0, 0, 0}};
    arp_header.target_hard_addr = vector<uint8_t>(empty_mac.octets, empty_mac.octets + sizeof(empty_mac.octets));
    arp_header.target_prot_addr = vector<uint8_t>(searched_ip.octets, searched_ip.octets + sizeof(searched_ip.octets));

    // set the next_header
    arp->next_header = arp_header;

    while (true) {
        // send REQUEST
        puts("send()");
        arp->send();

        // recv reply
        puts("waiting for reply");
        shared_ptr<struct arp_header>* reply = queue->queue.front(std::chrono::milliseconds(50)); // todo be able to control this value
        queue->queue.pop_front();
        if (reply == nullptr) continue; // didn't receive a response in a reasonable time, continue and send again

        // ensure it is a reply packet (no need, this is done directly via the listening registration)
        // if ((*reply)->op != ARP_OP_REPLY) continue;
        
        // ensure the ip reply is the searched_ip
        ip4_addr reply_ip;
        extract_from_network_order(&reply_ip, (*reply)->sender_prot_addr.data());
        if (reply_ip != searched_ip) continue;

        // success, found the mac addr of search_ip
        mac_addr res{};
        extract_from_network_order(&res, (*reply)->sender_hard_addr.data());
        
        queue->queue.clear(); // clear any trailing replies

        return res;
    }
}

vector<pair<ip4_addr, mac_addr>> arp_scanner::scan_entire_subnet(shared_ptr<network_access_bytes> const& broadcast_access, ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip) {
    // set arp over the broadcast access
    shared_ptr<arp_protocol> arp = make_shared<arp_protocol>();
    arp->set_net_access(broadcast_access);

    // listen to arp replies via a queue
    shared_ptr<recv_queue<shared_ptr<arp_header>>> queue = make_shared<recv_queue<shared_ptr<arp_header>>>();
    arp->listener_op[ARPOP_REPLY] = queue; // todo this value from my own list

    // send requests
    ip4_addr curr_ip = smallest_ip_addr_in_subnet(mask);
    while (true) {
        if (!is_inside_subnet(mask, curr_ip)) break;

        cout << "sending request for ip: " << ip4_addr_to_str(curr_ip) << endl;

        // arp header
        struct arp_header arp_header;
        arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
        arp_header.prot_type = ethertype::ip4;
        arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
        arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
        arp_header.op = ARPOP_REQUEST; // operation (request) // todo this value from my own list

        arp_header.sender_hard_addr = vector<uint8_t>(src_mac.octets, src_mac.octets + sizeof(src_mac.octets));
        arp_header.sender_prot_addr = vector<uint8_t>(src_ip.octets, src_ip.octets + sizeof(src_ip.octets));

        mac_addr empty_mac{{0, 0, 0, 0, 0, 0}};
        arp_header.target_hard_addr = vector<uint8_t>(empty_mac.octets, empty_mac.octets + sizeof(empty_mac.octets));
        arp_header.target_prot_addr = vector<uint8_t>(curr_ip.octets, curr_ip.octets + sizeof(curr_ip.octets));

        // set next_header and send request
        arp->next_header = arp_header;
        arp->send();

        // advance curr_ip
        curr_ip = generate_next_ip(curr_ip);

        // wait some dealy
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    cout << "sent all search requests completed, waiting and processing replies" << endl;

    // wait some reasonable time for all the replies
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // process all the replies
    vector<pair<ip4_addr, mac_addr>> res;
    while (!queue->queue.is_empty()) {
        shared_ptr<struct arp_header>* reply = queue->queue.front(std::chrono::milliseconds(0));
        queue->queue.pop_front();
        if (reply == nullptr) continue; // no reply?

        // ensure it is a reply packet (no need, this is done directly via the listening registration)
        // if ((*reply)->op != ARP_OP_REPLY) continue;
        
        // extract the ip4 addr
        ip4_addr reply_ip;
        extract_from_network_order(&reply_ip, (*reply)->sender_prot_addr.data());

        // ensure the ip reply is within the subnet mask
        if (!is_inside_subnet(mask, reply_ip)) continue;

        // extract the mac addr
        mac_addr reply_mac{};
        extract_from_network_order(&reply_mac, (*reply)->sender_hard_addr.data());

        // add to res
        res.emplace_back(reply_ip, reply_mac);
    }

    cout << "search completed, received " << res.size() << " entries" << endl;

    return res;
}


// arp_scanner::arp_scanner() : queue(make_shared<recv_queue<shared_ptr<arp_header>>>()) {
// }


// void arp_scanner::spoof_as_device(const std::vector<ip4_addr> &victims_ip, ip4_addr target_ip,
//                               mac_addr src_mac, ip4_addr src_ip) {
//     int no_victims = victims_ip.size();

//     vector<mac_addr> victim_macs(no_victims);
//     for (int i = 0; i < no_victims; i++)
//         victim_macs[i] = search_for_mac_addr(victims_ip[i], src_mac, src_ip);

// #define BUFF_SZ 65536
//     uint8_t buff[BUFF_SZ] = {0};

//     // prepare net_arp
//     struct ether_arp *arp_header = (struct ether_arp *) buff; // arp_header header
//     arp_header->arp_hrd = htons(ARPHRD_ETHER); // first net_arp octets type: mac
//     arp_header->arp_pro = htons(ethernet_header::ethertype_values::ip4); // first net_arp octets type: ip
//     arp_header->arp_hln = ETH_ALEN; // len of mac octets
//     arp_header->arp_pln = 4; // len of ip octets
//     arp_header->arp_op = htons(ARPOP_REPLY); // type of net_arp (forged reply)

//     write_in_network_order(arp_header->arp_sha, &src_mac); // my spoofed mac octets
//     write_in_network_order(arp_header->arp_spa, &target_ip);


//     while (true) {
//         std::this_thread::sleep_for(300ms);

//         if (victims_ip.empty()) { // spoof whole broadcast
//             memset(arp_header->arp_tha, 0xff, ETH_ALEN);
//             memset(arp_header->arp_tpa, 0x00, 4);

//             ether_client.next_dest_addr.set_next_choice(mac_addr_broadcast);
//             ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);

//             int cnt = sizeof(ether_arp);
//             send_msg_t request;
//             memcpy(request.get_active_buff(), buff, cnt);
//             request.set_count(cnt);
//             ether_client.send_data(std::move(request));

//         } else { // spoof every victim
//             for (int i = 0; i < no_victims; i++) {
//                 ip4_addr victim_ip = victims_ip[i];
//                 mac_addr victim_mac = victim_macs[i];

//                 // send to specific target_ip
//                 write_in_network_order(arp_header->arp_tha, &victim_mac);
//                 write_in_network_order(arp_header->arp_tpa, &victim_ip);

//                 ether_client.next_dest_addr.set_next_choice(victim_mac);
//                 ether_client.next_protocol.set_next_choice(ethernet_header::ethertype_values::arp);

//                 int cnt = sizeof(ether_arp);
//                 send_msg_t request;
//                 memcpy(request.get_active_buff(), buff, cnt);
//                 request.set_count(cnt);
//                 ether_client.send_data(std::move(request));
//             }

//         }

//         std::cout << "spoofed frames sent" << endl;
//     }

// }

// void net_arp::intercept_device_traffic(const vector<ip4_addr> &victim_devices, ip4_addr dest, bool block, mac_addr src_mac, ip4_addr src_ip) {
//     // block/allow traffic (linux)
//     if (block) system("echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null");
//     else system("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null");

//     spoof_as_device(victim_devices, dest, src_mac, src_ip);
// //    if (bidirectional)
// //        for (ip4_addr victim: victim_devices)
// //            spoof_as_device({dest}, victim, my_mac);

// }


void arp_scanner::announce_new_pair(shared_ptr<network_access_bytes> const& broadcast_access, ip4_addr new_ip, mac_addr new_mac) {
    // set arp over the broadcast access
    shared_ptr<arp_protocol> arp = make_shared<arp_protocol>();
    arp->set_net_access(broadcast_access);

    // arp header
    struct arp_header arp_header;
    arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
    arp_header.prot_type = ethertype::ip4;
    arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
    arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
    arp_header.op = ARPOP_REQUEST; // operation (request) // todo this value from my own list

    arp_header.sender_hard_addr = vector<uint8_t>(new_mac.octets, new_mac.octets + sizeof(new_mac.octets));
    arp_header.sender_prot_addr = vector<uint8_t>(new_ip.octets, new_ip.octets + sizeof(new_ip.octets));

    arp_header.target_hard_addr = vector<uint8_t>(mac_addr_empty.octets, mac_addr_empty.octets + sizeof(mac_addr_empty.octets));
    arp_header.target_prot_addr = vector<uint8_t>(ip4_addr_empty.octets, ip4_addr_empty.octets + sizeof(ip4_addr_empty.octets));
    
    // set next_header and send
    arp->next_header = arp_header;
    arp->send();

}
