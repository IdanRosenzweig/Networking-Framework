#include "linux_client_tcp.h"


linux_client_tcp::linux_client_tcp(const string &ip, int port) : ip(ip), port(port) {

}


void linux_client_tcp::conn() {
    fd = socket(AF_INET,
                    SOCK_STREAM,
                    IPPROTO_TCP);
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

    std::cout << "connecting to server..." << std::endl;
    if (connect(fd,
                (struct sockaddr *) &addr,
                sizeof(addr)
    ) < 0) {
        cerr << "can't connect to server" << endl;
        return;
    }

    cout << "connected successfully" << endl;

}

void linux_client_tcp::disconn() {
    close(fd);
}


int linux_client_tcp::send_encapsulated_data(void *buff, int count) {
    return send(fd, buff, count, 0);
}

int linux_client_tcp::recv_encapsulated_data(void *buff, int count) {
    return recv(fd, buff, count, 0);
}
