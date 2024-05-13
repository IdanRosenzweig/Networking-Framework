#include "ping_util.h"
#include <linux/icmp.h>

void ping_util::ping_node() {
    ip_client.next_dest_addr.set_next_choice(dest_ip.get_next_choice());
    ip_client.next_ttl.set_next_choice(255);

    char *data = "echo packet";
    int data_sz = strlen(data);

    icmp_client.next_type.set_next_choice(ICMP_ECHO);
    icmp_client.next_code.set_next_choice(0);
    icmp_header::content::echo_content content{0x1234, 0};
    icmp_client.next_content.set_next_choice(*(uint32_t*) &content);

    int i = 0;
    do {

        std::this_thread::sleep_for(delay_interval.get_next_choice());

        // count start time
        const auto start = std::chrono::high_resolution_clock::now();

        if (icmp_client.send_data({data, data_sz}) < 1) {
            std::cerr << "Failed to send packet" << std::endl;
            continue;
        }

        cout << "waiting for icmp reply" << endl;
        received_msg msg = raw_icmp.front();
        raw_icmp.pop_front();
        uint8_t *buf = msg.data.get() + msg.protocol_offsets.back().first;

        const auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        icmp_header *reply = reinterpret_cast<icmp_header *>(buf);

        bool failed = false;
        switch (reply->type) {
            case ICMP_TIME_EXCEEDED: {
                std::cout << "Time Exceeded" << std::endl;
                failed = true;
                break;
            }
            case ICMP_DEST_UNREACH: {
                std::cout << "Destination Unreachable" << std::endl;
                failed = true;
                break;
            }
            case ICMP_ECHOREPLY: {
                if (reply->content.echo.id != content.id) { failed = true; break;}
                std::cout << "Received icmp reply: seq=" << content.sequence
                          << " rrt=" << duration.count() << " millisecs" << std::endl;
                break;
            }
            default: failed = true;
        }
        if (failed) continue;

        content.sequence++;
        i++;

    } while (i < count.get_next_choice());

}
