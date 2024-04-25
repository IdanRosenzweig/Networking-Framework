#include "icmp_protocol.h"
#include <iostream>
#include <linux/icmp.h>
#include <cstring>
#include <thread>
#include "icmp_packet.h"
#include "../ip4/internet_checksum.h"

using namespace std;

void icmp_protocol::ping() {

#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    icmp_packet* packet = reinterpret_cast<icmp_packet *>(buff);
    packet->setup_echo();

    char *data = "echo packet";
    int data_sz = strlen(data);
    memcpy(buff + sizeof(icmp_packet), data, data_sz);

    int i = 0;
    do {
        packet->checksum = 0;
        packet->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_packet) + data_sz); // update checksum

        // count start time
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
        const auto start = std::chrono::high_resolution_clock::now();

        if (gateway->send_data({buff, (int) sizeof(icmp_packet) + data_sz}) < 1) {
            std::cerr << "Failed to send packet" << std::endl;
            continue;
        }

        while (true) {
            cout << "waiting for icmp reply" << endl;
            while (raw_message_q.empty()) { // todo optimize this

            }
            received_msg msg = raw_message_q.front();
            raw_message_q.pop_front();
            uint8_t * buf = msg.data.get() + msg.curr_offset;

            const auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            icmp_packet *reply = reinterpret_cast<icmp_packet *>(buf);

            bool failed = false;
            if (reply->content.echo.id != packet->content.echo.id) failed = true;
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
            }

            if (failed) {
                cout << "icmp reply err" << endl;
                continue;
            }

            std::cout << "Received icmp reply: seq=" << packet->content.echo.sequence
                      << " rrt=" << duration.count() << " microsec" << std::endl;
            break;
        }

    } while (++packet->content.echo.sequence < 10);
//    } while (++i < 10);

}

int icmp_protocol::send_data(send_msg msg) {

#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    icmp_packet *packet = reinterpret_cast<icmp_packet*>(buff);
    packet->setup_echo();
    packet->type = 80; // todo return

    memcpy(buff + sizeof(icmp_packet), msg.buff, msg.count);

    packet->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_packet) + msg.count);

    return gateway->send_data({buff, (int) sizeof(icmp_packet) + msg.count});
}

void icmp_protocol::handle_received_event(received_msg msg) {
    raw_message_q.push_back(msg);

    uint8_t *buff = msg.data.get() + msg.curr_offset;

    struct icmp_packet *icmp = (struct icmp_packet *) buff;
    uint8_t type = icmp->type;

    msg.protocol_offsets.push_back({msg.curr_offset, ICMP});
    msg.curr_offset += sizeof(struct icmp_packet);

    if (type_handlers.is_key_assigned(type)) {
        type_handlers.get_val_of_key(type)->handle_received_event(std::move(msg));
    }

}
