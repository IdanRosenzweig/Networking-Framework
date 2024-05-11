#include "traceroute_util.h"
#include <linux/icmp.h>
#include <linux/ip.h>

void traceroute_util::trace_to_destination() {
    ip_client.next_dest_addr.set_next_choice(dest_ip.get_next_choice());

    char *data = "echo packet";
    int data_sz = strlen(data);

    icmp_client.next_type.set_next_choice(ICMP_ECHO);
    icmp_client.next_code.set_next_choice(0);
    icmp_packet::content::echo_content content{0x1234, 0};
    icmp_client.next_content.set_next_choice(*(uint32_t *) &content);

    cout << "trace: " << endl;

    int curr_ttl = 1;
    while (true) {
        ip_client.next_ttl.set_next_choice(curr_ttl);

        if (icmp_client.send_data({data, data_sz}) < 1) {
            std::cerr << "Failed to send packet" << std::endl;
            continue;
        }


//        cout << "waiting for icmp reply" << endl;
        received_msg msg = raw_icmp.front();
        raw_icmp.pop_front();
        uint8_t *buf = msg.data.get() + msg.protocol_offsets.back().first;

        icmp_packet *reply = reinterpret_cast<icmp_packet *>(buf);

        bool completed = false;
        bool ttl_fail = false;
        bool err = false;
        switch (reply->type) {
            case ICMP_ECHOREPLY: {
                if (reply->content.echo.id != content.id) {
                    err = true;
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
                err = true;
        }
        if (err) continue;

        if (ttl_fail) {
            struct iphdr *ip_hdr = reinterpret_cast<iphdr *>(msg.data.get() +
                                                             (msg.protocol_offsets[msg.protocol_offsets.size() -
                                                                                   2]).first);
            ip4_addr ip;
            extract_from_network_order(&ip, reinterpret_cast<uint8_t *>(&ip_hdr->saddr));
            cout << convert_to_str(ip) << " -> " << endl;

            curr_ttl++;

        } else if (completed) {
            cout << convert_to_str(dest_ip.get_next_choice()) << endl;
            std::cout << "trace completed" << std::endl;
            break;
        }

    }

}
