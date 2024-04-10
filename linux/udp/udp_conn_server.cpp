#include "udp_conn_server.h"
#include <iostream>
using namespace std;

//sockaddr_in udp_conn_server::discover_next_host() {
//    // we are currently only interested in discovering the address of the client that sends the next data
//    struct sockaddr_in client_addr;
//    socklen_t len = sizeof(client_addr);
//
//    if (recvfrom(fd, nullptr, 0, MSG_PEEK, // this would provide us with the address of the client, without reading the data from the queue
//                 (struct sockaddr*)&client_addr, &len) == -1){
//        printf("Couldn't receive\n");
//        throw;
//    }
//    printf("Received message from IP: %s and dest_port: %i\n",
//           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
//
//    return client_addr;
//}

udp_conn_server::udp_conn_server(int port) : server_port(port) {

}


int udp_conn_server::recv_data(void* data, int count) {
//    struct udp_header* udp = static_cast<udp_header *>(data);
//    if (ntohs(udp->dest_port) != dest_port) {
//        cout << "received udp to dest_port not mine" << endl;
//        return;
//    }
//    last_port = ntohs(udp->source_port);
//
//    char* buff = (char*) data + sizeof(udp_header);
//    cout << "buff: " << buff << endl;

#define BUFF_LEN 512
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
        last_port = ntohs(udp->source_port);

        char* packet_data = (char*) buff + sizeof(udp_header);

        int copy_cnt = std::min(count, (int) (res - sizeof(udp_header)));
        memcpy(data, packet_data, count);
        return copy_cnt;
    }
}

void udp_conn_server::send_data(void* data, int cnt) {
#define BUFF_LEN 256
    char buff[BUFF_LEN];
    memset(buff, '\x00', BUFF_LEN);

    // udp header
    struct udp_header* udp = reinterpret_cast<udp_header *>(buff);

    udp->source_port = htons (server_port);
    udp->dest_port = htons (last_port);
    udp->len = htons(sizeof(udp_header) + cnt);
    udp->checksum = 0; // optional

    // data
    char* packet_data = buff + sizeof(udp_header);
    memcpy(packet_data, data, cnt);


    ip_server->setNextProt(IPPROTO_UDP);
//    ip_server->send_next_msg(ip_server->prot_handlers[IPPROTO_UDP].last_client,
//                                  buff, sizeof(udp_header) + cnt);
    ip_server->send_next_msg(ip_server->last_client,
                             buff, sizeof(udp_header) + cnt);
}