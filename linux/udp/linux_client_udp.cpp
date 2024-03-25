#include "linux_client_udp.h"
#include <iostream>
using namespace std;

std::unique_ptr<linux_cl_conn> linux_client_udp::conn_to_host() {
    int fd = socket(AF_INET,
                    SOCK_DGRAM,
                    IPPROTO_UDP);
    if (fd == -1) {
        cerr << "can't open socket" << endl;
        return nullptr;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\x00', sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET,
                  ip.c_str(),
                  (void *) &server_addr.sin_addr
    ) == -1) {
        cerr << "inet_pton error" << endl;
        return nullptr;
    }

    cout << "ready" << endl;

    auto *host = new linux_cl_conn;
    host->fd = fd; // my open fd
    host->addr = server_addr; // server's addr
    return std::unique_ptr<linux_cl_conn>(host);
}

linux_client_udp::linux_client_udp(const string &ip, int port) : ip(ip), port(port) {

}
