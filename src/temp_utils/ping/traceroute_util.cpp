#include "traceroute_util.h"
#include <linux/ip.h>
#include <linux/icmp.h>
#include <iostream>
using namespace std;

traceroute_util::traceroute_util(ip4_addr src_ip, gateway* gw) : network_layer_gw(gw) {

    // setup send flow
    ip_prot.send_medium.add_send_channel(network_layer_gw);
    ip_prot.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    icmp_prot.send_medium.add_send_channel(&ip_prot);

    // setup recv flow
    network_layer_gw->add_listener(&ip_prot);

    ip_prot.listeners.append_new_empty_handler(&icmp_prot);
    ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(IPPROTO_ICMP);

    icmp_prot.default_listener = this;
}

traceroute_util::~traceroute_util() {
    network_layer_gw->remove_listener(&ip_prot);
}

void traceroute_util::trace_to_destination() {
    ip_prot.next_dest_addr.set_next_choice(dest_ip.get_next_choice());

    const char *data = "echo packet";
    int data_sz = strlen(data);

    icmp_prot.next_type.set_next_choice(ICMP_ECHO);
    icmp_prot.next_code.set_next_choice(0);
    icmp_header::content::echo_content content{0x1234, 0};
    icmp_prot.next_content.set_next_choice(*(uint32_t *) &content);

    std::cout << "trace: " << endl;

    int curr_ttl = 1;
    while (true) {
        ip_prot.next_ttl.set_next_choice(curr_ttl);

        bool completed = false;
        bool ttl_fail = false;

        recv_queue.clear();
        received_msg* msg = nullptr;
        while (true) {
            // send
            send_msg send;
            memcpy(send.get_active_buff(), data, data_sz);
            send.set_count(data_sz);

            if (icmp_prot.send_data(std::move(send)) < 1) {
                std::cerr << "Failed to send packet" << std::endl;
                continue;
            }

            // recv
            msg = recv_queue.front<500>();
            if (msg == nullptr) {
                std::cerr << "no reply was received after reasonable time, sending again" << endl;
                continue;
            }
            recv_queue.clear();

            // handle response
            uint8_t *buf = msg->data.data() + msg->protocol_offsets.back().first;
            icmp_header *icmp_reply = reinterpret_cast<icmp_header *>(buf);

            bool failed = false;
            switch (icmp_reply->type) {
                case ICMP_ECHOREPLY: {
                    if (icmp_reply->content.echo.id != content.id) {
                        failed = true;
                        break;
                    }
                    completed = true;
                    break;
                }
                case ICMP_TIME_EXCEEDED: {
                    ttl_fail = true;
                    break;
                }
                default:
                    failed = true;
            }
            if (!failed) break;

        }

        if (ttl_fail) {
            struct iphdr *ip_hdr = reinterpret_cast<iphdr *>(msg->data.data() +
                                                             (msg->protocol_offsets[msg->protocol_offsets.size() -
                                                                                   2]).first);
            ip4_addr ip;
            extract_from_network_order(&ip, reinterpret_cast<uint8_t *>(&ip_hdr->saddr));
            std::cout << convert_to_str(ip) << " -> " << endl;

            curr_ttl++;

        } else if (completed) {
            std::cout << convert_to_str(dest_ip.get_next_choice()) << endl;
            std::cout << "trace completed" << std::endl;
            break;
        }
    }

}


