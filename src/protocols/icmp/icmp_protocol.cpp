#include "icmp_protocol.h"
#include <iostream>
#include <linux/icmp.h>
#include <cstring>
#include <thread>
#include "icmp_header.h"
#include "../ip4/internet_checksum.h"

using namespace std;

int icmp_protocol::send_data(send_msg msg) {

#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    icmp_header *packet = reinterpret_cast<icmp_header*>(buff);
    packet->type = next_type.get_next_choice();
    packet->code = next_code.get_next_choice();
    packet->content.raw = next_content.get_next_choice();

    memcpy(buff + sizeof(icmp_header), msg.buff, msg.count);

    packet->checksum = 0;
    packet->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_header) + msg.count);

    return gateway->send_data({buff, (int) sizeof(icmp_header) + msg.count});
}

void icmp_protocol::handle_received_event(received_msg& msg) {
//    raw_message_q.push_back(msg);

    uint8_t *buff = msg.data.get() + msg.curr_offset;

    struct icmp_header *icmp = (struct icmp_header *) buff;
    uint8_t type = icmp->type;

    msg.protocol_offsets.push_back({msg.curr_offset, ICMP});
    msg.curr_offset += sizeof(struct icmp_header);

    if (default_handler != nullptr)
        default_handler->handle_received_event(msg);

    if (type_handlers.is_key_assigned(type)) {
        type_handlers.get_val_of_key(type)->handle_received_event(msg);
//        return;
    }

}

//void icmp_protocol::ping_util() {
//
//#define BUFF_LEN 1024
//    char buff[BUFF_LEN];
//    memset(buff, '\x00', BUFF_LEN);
//
//    icmp_header* packet = reinterpret_cast<icmp_header *>(buff);
//    packet->type = ICMP_ECHO;
//    packet->code = 0;
//    packet->content.echo.id = 3333;
//    packet->content.echo.sequence = 0;
//
//    char *data = "echo packet";
//    int data_sz = strlen(data);
//    memcpy(buff + sizeof(icmp_header), data, data_sz);
//
//    int i = 0;
//    do {
////        using namespace std::chrono_literals;
////        std::this_thread::sleep_for(100ms);
//
//        packet->checksum = 0; // must reset the value before calculating the checksum again
//        packet->checksum = internet_checksum(reinterpret_cast<uint16_t *>(buff), sizeof(icmp_header) + data_sz); // update checksum
//
//        // count start time
//        const auto start = std::chrono::high_resolution_clock::now();
//
//        if (gateway->send_data({buff, (int) sizeof(icmp_header) + data_sz}) < 1) {
//            std::cerr << "Failed to send packet" << std::endl;
//            continue;
//        }
//
//        while (true) {
//            cout << "waiting for icmp reply" << endl;
//            received_msg msg = raw_message_q.front();
//            raw_message_q.pop_front();
//            uint8_t * buf = msg.data.get() + msg.curr_offset;
//
//            const auto end = std::chrono::high_resolution_clock::now();
//            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//
//            icmp_header *reply = reinterpret_cast<icmp_header *>(buf);
//
//            bool failed = false;
//            if (reply->content.echo.id != packet->content.echo.id) failed = true;
//            switch (reply->type) {
//                case ICMP_TIME_EXCEEDED: {
//                    std::cout << "Time Exceeded" << std::endl;
//                    failed = true;
//                    break;
//                }
//                case ICMP_DEST_UNREACH: {
//                    std::cout << "Destination Unreachable" << std::endl;
//                    failed = true;
//                    break;
//                }
//            }
//
//            if (failed) {
//                cout << "icmp reply err" << endl;
//                continue;
//            }
//
//            std::cout << "Received icmp reply: seq=" << packet->content.echo.sequence
//                      << " rrt=" << duration.count() << " millisecs" << std::endl;
//            break;
//        }
//
//    } while (++packet->content.echo.sequence < 1000);
////    } while (++i < 10);
//
//}