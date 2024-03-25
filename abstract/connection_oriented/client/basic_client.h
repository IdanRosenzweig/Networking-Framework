#ifndef SERVERCLIENT_BASIC_CLIENT_H
#define SERVERCLIENT_BASIC_CLIENT_H

#include "basic_server_handler.h"
#include <type_traits>

template <typename T>
class basic_client {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

private:
    basic_server_handler server;
protected:
    virtual std::unique_ptr<T> conn_host() = 0;

public:
    void conn_to_server() {
        std::unique_ptr<T> conn = std::move(this->conn_host());
        if (conn == nullptr) return;

        basic_server_handler handler;
        handler.net_host = std::unique_ptr<basic_conn>(
                static_cast<basic_conn*>(conn.release()));

        server = std::move(handler);
    }

    // this function may be overridden to create encapsulation
    virtual int send_data(void* buff, int count) {
        return ((basic_conn*) server.net_host.get())->send_data(buff, count);
    }

    // this function may be overridden to create encapsulation
    virtual int recv_data(void* buff, int count) {
        return ((basic_conn*) server.net_host.get())->recv_data(buff, count);
    }

};

#endif //SERVERCLIENT_BASIC_CLIENT_H
