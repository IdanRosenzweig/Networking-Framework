#include "linux_client_tcp.h"

std::unique_ptr<linux_co_conn<tcp_conn>> linux_client_tcp::conn_host() {
    int fd = socket(AF_INET,
                    SOCK_STREAM,
                    IPPROTO_TCP);
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

    std::cout << "connecting to server..." << std::endl;
    if (connect(fd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)
    ) < 0) {
        cerr << "can't connect to server" << endl;
        return nullptr;
    }

    cout << "connected successfully" << endl;

    auto *host = new linux_co_conn<tcp_conn>;
    host->fd = fd;
    return std::unique_ptr<linux_co_conn<tcp_conn>>(host);
}

linux_client_tcp::linux_client_tcp(const string &ip, int port) : ip(ip), port(port) {

}
