#include "ping_util.h"
#include "../../linux/hardware.h"
#include <linux/icmp.h>
#include <iostream>
using namespace std;

ping_util::ping_util(ip4_addr src_ip, gateway* gw) : network_layer_gw(gw) {
    // setup send flow
    ip_client.gateway = network_layer_gw;
    ip_client.next_protocol.set_next_choice(IPPROTO_ICMP);
    ip_client.next_source_addr.set_next_choice(src_ip);

    icmp_client.gateway = &ip_client;

    // setup recv flow
    network_layer_gw->add_listener(&ip_client);
    ip_client.protocol_handlers.assign_to_key(IPPROTO_ICMP, &icmp_client);

    icmp_client.default_handler = this;


    // default values
    count.set_next_choice(10);
    delay_interval.set_next_choice(10ms);
}

ping_util::~ping_util() {
    network_layer_gw->remove_listener(&ip_client);
}

void ping_util::ping_node() {
    ip_client.next_dest_addr.set_next_choice(dest_ip.get_next_choice());
    ip_client.next_ttl.set_next_choice(255);

    const char *data = "echo packet";
    int data_sz = strlen(data);

    icmp_client.next_type.set_next_choice(ICMP_ECHO);
    icmp_client.next_code.set_next_choice(0);
    icmp_header::content::echo_content content{0x1234, 0};
    icmp_client.next_content.set_next_choice(*(uint32_t*) &content);

    int i = 0;
    do {

        std::this_thread::sleep_for(delay_interval.get_next_choice());

        auto start = std::chrono::high_resolution_clock::now(); // count start time

        recv_queue.clear();
        while (true) {
            // send
            send_msg send;
            memcpy(send.get_active_buff(), data, data_sz);
            send.set_count(data_sz);

            if (icmp_client.send_data(std::move(send)) < 1) {
                std::cerr << "Failed to send packet" << std::endl;
                continue;
            }

            // recv
            std::cout << "sent icmp echo" << endl;

            received_msg* msg = recv_queue.front<500>();
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
                default:
                    failed = true;
            }

            if (!failed) break;

        }

        content.sequence++;
        icmp_client.next_content.set_next_choice(*(uint32_t*) &content);
        i++;

    } while (i < count.get_next_choice());

}

