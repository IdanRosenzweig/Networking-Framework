#include "linux_server_tcp.h"

std::unique_ptr<linux_co_conn> linux_server_tcp::conn_next_host() {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    // accepting a new client
    cout << "waiting for client..." << endl;
    int client_sd = accept(
            fd,
            (struct sockaddr *) &client_addr,
            &len
    );
    if (client_sd == -1) {
        cerr << "can't accept the client" << endl;
        return nullptr;
    }
    cout << "client accepted" << endl;

    auto *host = new linux_co_conn;
    host->fd = client_sd;
    return std::unique_ptr<linux_co_conn>(host);
}

linux_server_tcp::linux_server_tcp(int port) : port(port) {
    // opening a file descriptor
    fd = socket(
            AF_INET, // IPv4
            SOCK_STREAM,
            IPPROTO_TCP // TCP
    );
    if (fd == -1) {
        cerr << "socket err" << endl;
        return;
    }


    // binding a socket
    struct sockaddr_in addr;
    bzero((void *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(
            fd,
            (struct sockaddr *) &addr,
            sizeof(addr)
    ) == -1) {
        cerr << "bind err" << endl;
        return;
    }

    // listening on the socket
    if (listen(fd, 1) == -1) {
        cerr << "listen err";
        return;
    }

    cout << "listening on port: " << port << endl;
}
