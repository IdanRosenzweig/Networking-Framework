#ifndef SERVERCLIENT_BLOCK_TCP_H
#define SERVERCLIENT_BLOCK_TCP_H

#include <netinet/in.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>

#include "basic_firewall_filter.h"
#include "../utils/data_counter.h"

#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/ether/ethernet_protocol.h"

class block_tcp_filter : public basic_firewall_filter<received_msg> {
public:
    ethernet_protocol ether_prot;
    ip4_protocol ip_prot;

    class examiner : public msg_receiver {
        block_tcp_filter *master;
    public:
        explicit examiner(block_tcp_filter *master) : master(master) {}

    public:
        void handle_received_event(received_msg &event) override {
            struct tcphdr *tcp_hdr = reinterpret_cast<tcphdr *>(event.data.data() + event.curr_offset);
            int dest_port = ntohs(tcp_hdr->dest);
//            cout << "tcp examiner got " << dest_port << endl;
            if (dest_port == master->blocked_port) {
//                cout << "blocking" << endl;
                master->blocked_flag = true;
            }
        }

    } tcp_examine;

    int blocked_port;
    bool blocked_flag = false;

    block_tcp_filter(int port) : blocked_port(port), tcp_examine(this) {
        ether_prot.protocol_handlers.assign_to_key(htons(ETH_P_IP), &ip_prot);
        ip_prot.protocol_handlers.assign_to_key(IPPROTO_TCP, &tcp_examine);
    }

    firewall_policy calc_policy(received_msg &msg) override {
        ether_prot.handle_received_event(msg);

        if (blocked_flag) {
            blocked_flag = false;
//            cout << "policy set blocked" << endl;
            return BLOCK;
        }
        else return ALLOW;
    }
};

#endif //SERVERCLIENT_BLOCK_TCP_H
