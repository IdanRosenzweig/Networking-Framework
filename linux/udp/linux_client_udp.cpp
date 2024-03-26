#include "linux_client_udp.h"
#include <iostream>
using namespace std;

linux_client_udp::linux_client_udp(const string &ip, int port) : ip(ip), port(port) {

}

void linux_client_udp::init() {
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

void linux_client_udp::finish() {
    close(fd);
}

int linux_client_udp::send_encapsulated_data(void *buff, int count) {
    return sendto(fd, buff, count, 0, (struct sockaddr *) &addr, sizeof(addr));
}

int linux_client_udp::recv_encapsulated_data(void *buff, int count) {
    socklen_t len = sizeof(addr);
    return recvfrom(fd,
                    buff, count,
                    0,
                    (struct sockaddr *) &addr, &len);
}

