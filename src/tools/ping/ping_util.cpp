#include "ping_util.h"

#include <iostream>
using namespace std;

ping_util::ping_util(ip4_addr src_ip, gateway* gw) : network_layer_gw(gw) {
    // send flow
    prot_stack.ip_prot.send_medium.add_send_channel(network_layer_gw);
    prot_stack.ip_prot.next_protocol.set_next_choice(ip4_header::protocol_values::icmp);
    prot_stack.ip_prot.next_source_addr.set_next_choice(src_ip);

    prot_stack.icmp_prot.send_medium.add_send_channel(&prot_stack.ip_prot);

    // recv flow
    network_layer_gw->add_listener(&prot_stack.ip_prot);

    prot_stack.ip_prot.listeners.append_new_empty_handler(&prot_stack.icmp_prot);
    prot_stack.ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(ip4_header::protocol_values::icmp);

    prot_stack.icmp_prot.default_listener = &replies_recv_q;

    // default values
    count.set_next_choice(10);
    delay_interval.set_next_choice(10ms);
}

ping_util::~ping_util() {
    network_layer_gw->remove_listener(&prot_stack.ip_prot);
}

void ping_util::ping_node() {
    prot_stack.ip_prot.next_dest_addr.set_next_choice(dest_ip.get_next_choice());
    prot_stack.ip_prot.next_ttl.set_next_choice(255);

    const char *data = "echo packet";
    int data_sz = strlen(data);

    prot_stack.icmp_prot.next_type.set_next_choice(icmp_header::type_values::echo);
    prot_stack.icmp_prot.next_code.set_next_choice(0);
    icmp_header::content::echo_content content{0x1234, 0};
    prot_stack.icmp_prot.next_content.set_next_choice(*(uint32_t*) &content);

    int i = 0;
    do {

        auto start = std::chrono::high_resolution_clock::now(); // count start time

        replies_recv_q.queue.clear();
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
            std::cout << "sent icmp echo" << endl;

            recv_msg_t* msg = replies_recv_q.queue.front<2000>();
            if (msg == nullptr) {
                std::cerr << "no reply was received after reasonable time, sending again" << endl;
                continue;
            }
            replies_recv_q.queue.clear();

            // handle response
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            struct icmp_header icmp_reply;
            extract_from_network_order(&icmp_reply, msg->buff_of_prot_from_end(0));

            bool failed = false;
            switch (icmp_reply.type) {
                case icmp_header::type_values::time_exceeded: {
                    std::cerr << "received time Exceeded" << std::endl;
                    failed = true;
                    break;
                }
                case icmp_header::type_values::dest_unreachable: {
                    std::cerr << "received destination unreachable" << std::endl;
                    failed = true;
                    break;
                }
                case icmp_header::type_values::redirect: {
                    std::cerr << "received redirect" << std::endl;
                    failed = true;
                    break;
                }
                case icmp_header::type_values::echoreply: {
                    if (icmp_reply.content.echo.id != content.id) {
                        std::cerr << "received icmp reply with non-matching id" << std::endl;
                        failed = true;
                        break;
                    }
                    std::cout << "received icmp reply\tseq_num=" << content.sequence
                              << "\tdelay=" << duration.count() << " millisecs" << std::endl;
                    break;
                }
                default: {
                    std::cerr << "received unknown type of icmp" << std::endl;
                    failed = true;
                    break;
                }
            }

            if (!failed) break;
        }

        content.sequence++;
        prot_stack.icmp_prot.next_content.set_next_choice(*(uint32_t*) &content);
        i++;

        std::this_thread::sleep_for(delay_interval.get_next_choice());

    } while (i < count.get_next_choice());

}

