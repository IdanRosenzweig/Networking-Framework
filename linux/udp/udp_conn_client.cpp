#include "udp_conn_client.h"
#include <iostream>
using namespace std;

udp_conn_client::udp_conn_client(const string &ip, int port) : ip(ip), port(port) {

}

void udp_conn_client::init() {
    // this whole function count have been simply ip4.init(ip) if we used it...

    fd = socket(AF_INET,
                    SOCK_DGRAM,
                    IPPROTO_UDP);
    if (fd == -1) {
        cerr << "can't open socket" << endl;
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET,
                  ip.c_str(),
                  (void *) &addr.sin_addr
    ) == -1) {
        cerr << "inet_pton error" << endl;
        return;
    }

}

void udp_conn_client::finish() {
    close(fd);
}

int udp_conn_client::send_encapsulated_data(void *buff, int count) {
    return sendto(fd, buff, count, 0, (struct sockaddr *) &addr, sizeof(addr));
}

int udp_conn_client::recv_encapsulated_data(void *buff, int count) {
    socklen_t len = sizeof(addr);
    return recvfrom(fd,
                    buff, count,
                    0,
                    (struct sockaddr *) &addr, &len);
}

