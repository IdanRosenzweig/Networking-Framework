#include "arp_scanner.h"

#include "src/protocols/ether/ethertype.h"

#include <linux/if_arp.h>

arp_scanner::arp_scanner() : queue(make_shared<recv_queue<shared_ptr<arp_header>>>()) {
}

void arp_scanner::set_arp(shared_ptr<arp_protocol> const& _arp) {
    arp = _arp;

    arp->listener_op[ARPOP_REPLY] = queue; // todo this value from my own list
}

mac_addr arp_scanner::search_for_mac_addr(ip4_addr const& searched_ip, mac_addr const& src_mac, ip4_addr const& src_ip) {
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
    arp_header.sender_hard_addr = vector<uint8_t>(empty_mac.octets, empty_mac.octets + sizeof(empty_mac.octets));
    arp_header.sender_prot_addr = vector<uint8_t>(searched_ip.octets, searched_ip.octets + sizeof(searched_ip.octets));

    // set the next_header
    arp->next_header = arp_header;

    while (true) {
        // send REQUEST
        arp->send();

        // recv reply
        shared_ptr<struct arp_header>* reply = queue->queue.front(std::chrono::milliseconds(50)); // todo be able to control this value
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