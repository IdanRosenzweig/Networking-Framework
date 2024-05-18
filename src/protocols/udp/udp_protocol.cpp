#include "udp_protocol.h"
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "../ip4/internet_checksum.h"
using namespace std;


int udp_protocol::send_data(send_msg<>& msg) {
    uint8_t* buff = msg.get_reserve_buff();

    // udp_client_server header
    struct udp_header *udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons(next_source_port.get_next_choice());
    udp->dest_port = htons(next_dest_port.get_next_choice());
    udp->len = htons(sizeof(udp_header) + msg.get_count());
    udp->checksum = 0;

    // msg
    uint8_t *packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, msg.get_active_buff(), msg.get_count());

    msg.toggle_active_buff();
    msg.set_count(sizeof(udp_header) + msg.get_count());
    return gateway->send_data(msg);
}

void udp_protocol::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.data() + msg.curr_offset;

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