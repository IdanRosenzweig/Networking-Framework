#ifndef SERVERCLIENT_SERVER_H
#define SERVERCLIENT_SERVER_H



#include <queue>


class basic_cl_server {

protected:
    // discover next host trying to send us data
    virtual sockaddr_in discover_next_host() = 0;

public:
    std::queue<sockaddr_in> clients_q;

    void discover_next_client() {
        sockaddr_in addr = std::move(this->discover_next_host());
        clients_q.push(addr);
    }

};

#endif //SERVERCLIENT_SERVER_H
