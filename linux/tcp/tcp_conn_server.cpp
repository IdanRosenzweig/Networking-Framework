#include "tcp_conn_server.h"

handler tcp_conn_server::conn_next_host() {
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
        throw;
    }
    cout << "client accepted" << endl;

    return {client_sd, client_addr};
}

tcp_conn_server::tcp_conn_server(int port) : port(port) {

}

void tcp_conn_server::setup() {
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

    cout << "listening on server_port: " << port << endl;
}

void tcp_conn_server::destroy() {
    close(fd);
}

