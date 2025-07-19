#ifndef NETWORKING_BLOCK_INCOMING_TCP_H
#define NETWORKING_BLOCK_INCOMING_TCP_H

#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>

#include "../../abstract/firewall/basic_firewall_filter.h"
#include "../../utils/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet2_protocol.h"

class block_incoming_tcp : public basic_firewall_filter<recv_msg_t> {
    ethernet2_protocol ether_prot;
    ip4_protocol ip_prot;

    class examiner : public msg_recv_listener {
        block_incoming_tcp *master;
    public:
        explicit examiner(block_incoming_tcp *master) : master(master) {}

    public:
        void handle_callback(recv_msg_t &&data) override {
            struct tcphdr *tcp_hdr = reinterpret_cast<tcphdr *>(data.buff_at_curr_offset());
            uint16_t dest_port = ntohs(tcp_hdr->dest);
//            std::cout << "tcp examiner got " << dest_port << endl;
            if (dest_port == master->blocked_port) {
//                std::cout << "blocking" << endl;
                master->blocked_flag = true;
            }
        }

    } tcp_examine;

    uint16_t blocked_port;
    bool blocked_flag = false;

public:
    block_incoming_tcp(uint16_t port) : blocked_port(port), tcp_examine(this) {
        ether_prot.listeners.append_new_empty_handler(&ip_prot);
        ether_prot.listeners.add_requirement_to_last_handler<ETHER_LISTEN_ON_PROTOCOL_INDEX>(ethernet_header::ethertype_values::ip4);

        ip_prot.listeners.append_new_empty_handler(&tcp_examine);
        ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::tcp);
    }

    firewall_policy calc_policy(const recv_msg_t &msg) override {
        recv_msg_t copy = msg;
        ether_prot.handle_callback(std::move(copy));

        if (blocked_flag) {
            blocked_flag = false;
//            std::cout << "policy set blocked" << endl;
            return BLOCK;
        }
        else return ALLOW;
    }
};

#endif //NETWORKING_BLOCK_INCOMING_TCP_H
