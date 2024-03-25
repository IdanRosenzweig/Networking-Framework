#ifndef SERVERCLIENT_BASIC_SERVER_H
#define SERVERCLIENT_BASIC_SERVER_H


#include "basic_client_handler.h"
#include <type_traits>
#include <vector>

template <typename T>
class basic_server {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

protected:
    // connect next host
    virtual std::unique_ptr<T> conn_next_host() = 0;

public:
    std::vector<basic_client_handler> clients;

    void accept_next_client() {
        std::unique_ptr<T> conn = std::move(this->conn_next_host());
        if (conn == nullptr) return;

        basic_client_handler handler;
        handler.net_host = std::unique_ptr<basic_conn>(
                static_cast<basic_conn*>(conn.release()));
        handler.id = 1;

        clients.push_back(std::move(handler));
    }
};

#endif //SERVERCLIENT_BASIC_SERVER_H
