#include "traceroute_util.h"

#include <iostream>
using namespace std;

traceroute_util::traceroute_util(ip4_addr src_ip, gateway* gw) : network_layer_gw(gw) {

    // setup send flow
    prot_stack.ip_prot.send_medium.add_send_channel(network_layer_gw);
    prot_stack.ip_prot.next_protocol.set_next_choice(ip4_header::protocol_values::icmp);
    prot_stack.ip_prot.next_source_addr.set_next_choice(src_ip);

    prot_stack.icmp_prot.send_medium.add_send_channel(&prot_stack.ip_prot);

    // setup recv flow
    network_layer_gw->add_listener(&prot_stack.ip_prot);

    prot_stack.ip_prot.listeners.append_new_empty_handler(&prot_stack.icmp_prot);
    prot_stack.ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::icmp);

    prot_stack.icmp_prot.default_listener = &replies_recv_q;
}

traceroute_util::~traceroute_util() {
    network_layer_gw->remove_listener(&prot_stack.ip_prot);
}

void traceroute_util::trace_to_destination() {
    prot_stack.ip_prot.next_dest_addr.set_next_choice(dest_ip.get_next_choice());

    const char *data = "echo packet";
    int data_sz = strlen(data);

    prot_stack.icmp_prot.next_type.set_next_choice(icmp_header::type_values::echo);
    prot_stack.icmp_prot.next_code.set_next_choice(0);
    icmp_header::content::echo_content content{0x1234, 0};
    prot_stack.icmp_prot.next_content.set_next_choice(*(uint32_t *) &content);

    std::cout << "trace: " << endl;

    int curr_ttl = 1;
    while (true) {
        prot_stack.ip_prot.next_ttl.set_next_choice(curr_ttl);

        bool completed = false;
        bool ttl_fail = false;

        replies_recv_q.queue.clear();
        recv_msg_t* msg = nullptr;
        while (true) {
            // send
            send_msg_t send;
            memcpy(send.get_active_buff(), data, data_sz);
            send.set_count(data_sz);

            if (prot_stack.icmp_prot.send_data(std::move(send)) < 1) {
                std::cerr << "Failed to send packet" << std::endl;
                continue;
            }

            // recv
            msg = replies_recv_q.queue.front<500>();
            if (msg == nullptr) {
                std::cerr << "no reply was received after reasonable time, sending again" << endl;
                continue;
            }
            replies_recv_q.queue.clear();

            // handle response
            struct icmp_header icmp_reply;
            extract_from_network_order(&icmp_reply, msg->buff_of_prot_from_end(0));

            bool failed = false;
            switch (icmp_reply.type) {
                case icmp_header::type_values::echoreply: {
                    if (icmp_reply.content.echo.id != content.id) {
                        failed = true;
                        break;
                    }
                    completed = true;
                    break;
                }
                case icmp_header::type_values::time_exceeded: {
                    ttl_fail = true;
                    break;
                }
                default:
                    failed = true;
            }
            if (!failed) break;

        }

        if (ttl_fail) {
            struct ip4_header ip_header;
            extract_from_network_order(&ip_header, msg->buff_of_prot_from_end(1));
            std::cout << convert_to_str(ip_header.source_addr) << " -> " << endl;

            curr_ttl++;

        } else if (completed) {
            std::cout << convert_to_str(dest_ip.get_next_choice()) << endl;
            std::cout << "trace completed" << std::endl;
            break;
        }
    }

}


