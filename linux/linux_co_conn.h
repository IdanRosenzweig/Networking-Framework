#ifndef SERVERCLIENT_LINUX_CO_CONN_H
#define SERVERCLIENT_LINUX_CO_CONN_H

#include "../abstract/basic_conn.h"

// from server pov,
// every type of connection-oriented communication in linux can be read/written through using only the client's associated open file descriptor.

// from client pov,
// every type of connection-oriented communication in linux can be read/written through using only my open file descriptor.

// there is no need to implement a class for every different protocol.
template <typename T>
class linux_co_conn : public T {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

public:
    // in server side, this represents the client's open fd.
    // in client side, this represents my open fd.
    int fd;

    int send_data(void *buff, int count) override {
        int i = 0;
        while (i < count) {
            char ch = ((char*) buff)[i];

            if (write(fd, (void*) &ch, 1) == 0) break; // todo change to != 1

            i++;
        }

        return i;
    }

    int recv_data(void *buff, int count) override {
        int i = 0;
        while (i < count) {
            char ch;
            memset((void*) &ch, 'z', 1);

            if (read(fd, (void*) &ch, 1) == 0) break; // todo change to != 1

            ((char*) buff)[i] = ch;
            i++;
        }

        return i;
    }
};


#endif //SERVERCLIENT_LINUX_CO_CONN_H
