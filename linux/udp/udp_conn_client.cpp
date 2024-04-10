#include "udp_conn_client.h"
#include <iostream>
#include "udp_header.h"
using namespace std;

udp_conn_client::udp_conn_client(int dest_port, int my_port) : dest_port(dest_port), my_port(my_port) {

}


int udp_conn_client::recv_data(void* data, int count) {
//    struct udp_header* udp = static_cast<udp_header *>(data);
//    if (ntohs(udp->dest_port) != dest_port) {
//        cout << "received udp to dest_port not mine" << endl;
//        return;
//    }
//    last_port = ntohs(udp->source_port);
//
//    char* buff = (char*) data + sizeof(udp_header);
//    cout << "buff: " << buff << endl;

#define BUFF_LEN 256
    char buff[BUFF_LEN];
    while (true) {

        memset(buff, '\x00', BUFF_LEN);
        ip_client->setNextProt(IPPROTO_UDP);
        int res = ip_client->recv_next_msg(buff, BUFF_LEN);

        struct udp_header* udp = (udp_header *) (buff);
        if (ntohs(udp->dest_port) != my_port) {
            cout << "recevied data not to my port: " << ntohs(udp->dest_port) << endl;
            continue; // keep on receiving
        }

        char* packet_data = (char*) buff + sizeof(udp_header);

        int copy_cnt = std::min(count, (int) (res - sizeof(udp_header)));
        memcpy(data, packet_data, copy_cnt);
        return copy_cnt;
    }

}

int udp_conn_client::send_data(void* data, int cnt) {
#define BUFF_LEN 256
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    // udp header
    struct udp_header* udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons (my_port);
    udp->dest_port = htons (dest_port);
    udp->len = htons(sizeof(udp_header) + cnt);
    udp->checksum = 0; // optional

    // data
    char* packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, data, cnt);


    ip_client->setNextProt(IPPROTO_UDP);
    return ip_client->send_next_msg(buff, sizeof(udp_header) + cnt);
}