#include "tcp_conn_client.h"


tcp_conn_client::tcp_conn_client(const string &ip, int port) : ip(ip), port(port) {

}


void tcp_conn_client::conn() {
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

void tcp_conn_client::disconn() {
    close(fd);
}


int tcp_conn_client::send_data(void *buff, int cnt) {
    send(fd, buff, cnt, 0);
}

int tcp_conn_client::recv_data(void *data, int count) {
    recv(fd, data, count, 0);
}
