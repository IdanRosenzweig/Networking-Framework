#include "linux_server_udp.h"
#include <iostream>
using namespace std;

sockaddr_in linux_server_udp::discover_next_host() {
    // we are currently only interested in discovering the address of the client that sends the next data
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    if (recvfrom(fd, nullptr, 0, MSG_PEEK, // this would provide us with the address of the client, without reading the data from the queue
                 (struct sockaddr*)&client_addr, &len) == -1){
        printf("Couldn't receive\n");
        throw;
    }
    printf("Received message from IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    return client_addr;
}

linux_server_udp::linux_server_udp(int port) : port(port) {

}

void linux_server_udp::setup() {
// opening a file descriptor
    fd = socket(
            AF_INET, // IPv4
            SOCK_DGRAM,
            IPPROTO_UDP // UDP
    );
    if (fd == -1) {
        cerr << "socket err" << endl;
        return;
    }

    // binding a socket with the port
    struct sockaddr_in addr;
    memset((void *) &addr, '\x00', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd,
             (struct sockaddr *) &addr,
             sizeof(addr)
    ) == -1) {
        cerr << "bind err" << endl;
        return;
    }

    cout << "waiting on port: " << port << endl;
}

void linux_server_udp::destroy() {
    close(fd);
}

int linux_server_udp::send_encapsulated_data(void *buff, int count, sockaddr_in addr) {
    return sendto(fd, buff, count, 0, (struct sockaddr*)&addr, sizeof(addr));
}

int linux_server_udp::recv_encapsulated_data(void *buff, int count, sockaddr_in addr) {
    socklen_t len = sizeof(addr);
    return recvfrom(fd, buff, count, 0, (struct sockaddr*)&addr, &len);
}
