#include "ping_util.h"
#include "../../linux/if/hardware.h"
#include <linux/icmp.h>
#include <iostream>
using namespace std;

ping_util::ping_util(ip4_addr src_ip, gateway* gw) : network_layer_gw(gw) {
    // send flow
    ip_prot.send_medium.add_send_channel(network_layer_gw);
    ip_prot.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_prot.next_source_addr.set_next_choice(src_ip);

    icmp_prot.send_medium.add_send_channel(&ip_prot);

    // recv flow
    network_layer_gw->add_listener(&ip_prot);

    ip_prot.listeners.append_new_empty_handler(&icmp_prot);
    ip_prot.listeners.add_requirement_to_last_handler<IP4_LISTEN_ON_PROTOCOL_INDEX>(IPPROTO_ICMP);

    icmp_prot.default_listener = this;

    // default values
    count.set_next_choice(10);
    delay_interval.set_next_choice(10ms);
}

ping_util::~ping_util() {
    network_layer_gw->remove_listener(&ip_prot);
}

void ping_util::ping_node() {
    ip_prot.next_dest_addr.set_next_choice(dest_ip.get_next_choice());
    ip_prot.next_ttl.set_next_choice(255);

    const char *data = "echo packet";
    int data_sz = strlen(data);

    icmp_prot.next_type.set_next_choice(ICMP_ECHO);
    icmp_prot.next_code.set_next_choice(0);
    icmp_header::content::echo_content content{0x1234, 0};
    icmp_prot.next_content.set_next_choice(*(uint32_t*) &content);

    int i = 0;
    do {

        auto start = std::chrono::high_resolution_clock::now(); // count start time

        recv_queue.clear();
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
            std::cout << "sent icmp echo" << endl;

            received_msg* msg = recv_queue.front<2000>();
            if (msg == nullptr) {
                std::cerr << "no reply was received after reasonable time, sending again" << endl;
                continue;
            }
            recv_queue.clear();

            // handle response
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            uint8_t *buf = msg->data.data() + msg->protocol_offsets.back().first;
            icmp_header *icmp_reply = reinterpret_cast<icmp_header *>(buf);

            bool failed = false;
            switch (icmp_reply->type) {
                case ICMP_TIME_EXCEEDED: {
                    std::cerr << "received time Exceeded" << std::endl;
                    failed = true;
                    break;
                }
                case ICMP_DEST_UNREACH: {
                    std::cerr << "received destination unreachable" << std::endl;
                    failed = true;
                    break;
                }
                case ICMP_ECHOREPLY: {
                    if (icmp_reply->content.echo.id != content.id) {
                        std::cerr << "received icmp reply with non-matching id" << std::endl;
                        failed = true;
                        break;
                    }
                    std::cout << "received icmp reply\tseq_num=" << content.sequence
                              << "\tdelay=" << duration.count() << " millisecs" << std::endl;
                    break;
                }
                default: {
                    std::cerr << "received unsupported type of icmp" << std::endl;
                    failed = true;
                }
            }

            if (!failed) break;

        }

        content.sequence++;
        icmp_prot.next_content.set_next_choice(*(uint32_t*) &content);
        i++;

        std::this_thread::sleep_for(delay_interval.get_next_choice());

    } while (i < count.get_next_choice());

}

