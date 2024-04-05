#include "udp_conn_client.h"
#include <iostream>
#include "udp_header.h"
using namespace std;

udp_conn_client::udp_conn_client(int port) : server_port(port) {

}

void udp_conn_client::init() {
    // this whole function count have been simply ip4.init(ip) if we used it...

//    fd = socket(AF_INET,
//                    SOCK_DGRAM,
//                    IPPROTO_UDP);
//    if (fd == -1) {
//        cerr << "can't open socket" << endl;
//        return;
//    }
//
//    addr.sin_family = AF_INET;
//    addr.sin_port = htons(server_port);
//    if (inet_pton(AF_INET,
//                  ip.c_str(),
//                  (void *) &addr.sin_addr
//    ) == -1) {
//        cerr << "inet_pton error" << endl;
//        return;
//    }

}

void udp_conn_client::finish() {
//    close(fd);
}

int udp_conn_client::recv_data(void* data, int count) {
//    struct udp_header* udp = static_cast<udp_header *>(data);
//    if (ntohs(udp->dest_port) != server_port) {
//        cout << "received udp to server_port not mine" << endl;
//        return;
//    }
//    last_port = ntohs(udp->source_port);
//
//    char* buff = (char*) data + sizeof(udp_header);
//    cout << "buff: " << buff << endl;

#define BUFF_LEN 256
    char buff[BUFF_LEN];
    while (true) {

        memset(&buff, '\x00', BUFF_LEN);
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

void udp_conn_client::send_data(void* data, int cnt) {
#define BUFF_LEN 256
    char buff[BUFF_LEN];
    memset(&buff, '\x00', BUFF_LEN);

    // udp header
    struct udp_header* udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons (my_port);
    udp->dest_port = htons (server_port);
    udp->len = htons(sizeof(udp_header) + cnt);
    udp->checksum = 0; // optional

    // data
    char* packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, data, cnt);


    ip_client->setNextProt(IPPROTO_UDP);
    ip_client->send_next_msg(buff, sizeof(udp_header) + cnt);
}