#include "udp_protocol.h"
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "../ip4/internet_checksum.h"
using namespace std;


int udp_protocol::send_data(send_msg msg) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    // udp_client_server header
    struct udp_header *udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons(next_source_port.get_next_choice());
    udp->dest_port = htons(next_dest_port.get_next_choice());
    udp->len = htons(sizeof(udp_header) + msg.count);
    udp->checksum = 0;

    // msg
    char *packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, msg.buff, msg.count);

    return gateway->send_data({buff, (int) sizeof(udp_header) + msg.count});
}

void udp_protocol::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;

    struct udp_header *udp = (struct udp_header *) buff;
    uint16_t port = ntohs(udp->dest_port);


    msg.protocol_offsets.push_back({msg.curr_offset, UDP});
    msg.curr_offset += sizeof(struct udp_header);

//    cout << "udp_client_server aggregator app_handler called on port " << port << endl;

    if (default_handler != nullptr)
        default_handler->handle_received_event(msg);

    if (port_handlers.is_key_assigned(port)) {
//        cout << "found assigned port: " << port << endl;
         port_handlers.get_val_of_key(port)->handle_received_event(msg);
//         return;
    }

}