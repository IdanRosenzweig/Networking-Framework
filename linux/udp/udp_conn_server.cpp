#include "udp_conn_server.h"
#include <iostream>
using namespace std;


udp_conn_server::udp_conn_server(int port) : server_port(port) {

}


int udp_conn_server::recv_data(void* data, int count) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    while (true) {
        memset(buff, '\x00', BUFF_LEN);
        ip_server->setNextProt(IPPROTO_UDP);
        int res = ip_server->recv_next_msg(buff, BUFF_LEN);

        struct udp_header* udp = (udp_header *) (buff);
        if (ntohs(udp->dest_port) != server_port) {
            cout << "server recieved to wrong port: " << ntohs(udp->dest_port) << endl;
            continue; // keep on receiving
        }
        cout << "server recieved port: " << ntohs(udp->dest_port) << endl;
//        last_port = ntohs(udp->source_port);

        char* packet_data = (char*) buff + sizeof(udp_header);

        int copy_cnt = std::min(count, (int) (res - sizeof(udp_header)));
        memcpy(data, packet_data, count);
        return copy_cnt;
    }
}

void udp_conn_server::send_data(void* data, int cnt) {
#define BUFF_LEN 1024
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    // udp header
    struct udp_header* udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons (server_port);
    udp->dest_port = htons (next_port);
    udp->len = htons(sizeof(udp_header) + cnt);
    udp->checksum = 0; // optional

    // data
    char* packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, data, cnt);


    ip_server->setNextProt(IPPROTO_UDP);
//    ip_server->send_next_msg(ip_server->prot_handlers[IPPROTO_UDP].last_client,
//                                  buff, sizeof(udp_header) + cnt);
    ip_server->send_next_msg(buff, sizeof(udp_header) + cnt);
}