#ifndef SERVERCLIENT_LINUX_CL_CONN_H
#define SERVERCLIENT_LINUX_CL_CONN_H

#include "../abstract/basic_conn.h"

// from server pov,
// every type of connectionless communication in linux can be read/written through using only the server's open file descriptor and the client's socket address.

// from client pov,
// every type of connectionless communication in linux can be read/written through using only my open file descriptor and the server's socket address.

// there is no need to implement a class for every different protocol.
template<typename T>
class linux_cl_conn : public T {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

public:
    // in server side, this represents the server's open fd.
    // in client side, this represents my open fd.
    int fd;

    // in server side, this represents the client's addr.
    // in client side, this represents the server's addr
    struct sockaddr_in addr;


    int send_data(void *buff, int count) override {
        return sendto(fd,
                      buff, count,
                      0,
                      (struct sockaddr *) &addr, sizeof(addr));

    }

    int recv_data(void *buff, int count) override {
        socklen_t len = sizeof(addr);
        return recvfrom(fd,
                        buff, count,
                        0,
                        (struct sockaddr *) &addr, &len);
    }
};

#endif //SERVERCLIENT_LINUX_CL_CONN_H
