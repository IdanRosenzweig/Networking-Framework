#include "udp_protocol.h"
#include <iostream>
#include "udp_header.h"
#include <netinet/in.h>
#include <cstring>

using namespace std;


int udp_protocol::send_data(send_msg msg) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    // udp header
    struct udp_header *udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons(next_source_port.get_next_choice());
    udp->dest_port = htons(next_dest_port.get_next_choice());
    udp->len = htons(sizeof(udp_header) + msg.count);
    udp->checksum = 0; // optional

    // msg
    char *packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, msg.buff, msg.count);


//    gateway->next_protocol.set_next_choice(IPPROTO_UDP);
//    gateway->send_next_msg(gateway->prot_handlers[IPPROTO_UDP].last_client,
//                                  buff, sizeof(udp_header) + cnt);
    return gateway->send_data({buff, (int) sizeof(udp_header) + msg.count});
}

void udp_protocol::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;

    struct udp_header *udp = (struct udp_header *) buff;
    uint16_t port = ntohs(udp->dest_port);


    msg.protocol_offsets.push_back({msg.curr_offset, UDP});
    msg.curr_offset += sizeof(struct udp_header);

//    cout << "udp aggregator handler called on port " << port << endl;

    if (port_handlers.is_key_assigned(port)) {
//        cout << "found assigned port: " << port << endl;
         port_handlers.get_val_of_key(port)->handle_received_event(msg);
         return;
    }

    if (default_handler != nullptr)
        default_handler->handle_received_event(msg);

}