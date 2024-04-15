#include "udp_conn_server.h"
#include <iostream>
#include "udp_header.h"
#include <netinet/in.h>
#include <cstring>

using namespace std;


int udp_conn_server::send_data(void *data, int cnt) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    // udp header
    struct udp_header *udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons(server_port.get_next_choice());
    udp->dest_port = htons(next_external_port.get_next_choice());
    udp->len = htons(sizeof(udp_header) + cnt);
    udp->checksum = 0; // optional

    // data
    char *packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, data, cnt);


//    gateway->next_external_protocol.set_next_choice(IPPROTO_UDP);
//    gateway->send_next_msg(gateway->prot_handlers[IPPROTO_UDP].last_client,
//                                  buff, sizeof(udp_header) + cnt);
    return gateway->send_data(buff, sizeof(udp_header) + cnt);
}

void udp_conn_server::handle_received_msg(msg_protocol_stack &&msg) {
    uint8_t *buff = msg.msg.data.get() + msg.curr_offset;

    struct udp_header *udp = (struct udp_header *) buff;
    uint16_t port = ntohs(udp->dest_port);

    msg.protocol_offsets.push(msg.curr_offset);

//    cout << "udp server handler called on port " << port << endl;

    if (port_handlers.count(port)) {
        msg.curr_offset += sizeof(struct udp_header);
        port_handlers[port]->handle_received_msg(std::move(msg));
    }

}