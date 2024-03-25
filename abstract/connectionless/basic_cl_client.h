#ifndef SERVERCLIENT_BASIC_CL_CLIENT_H
#define SERVERCLIENT_BASIC_CL_CLIENT_H

#include "../basic_conn.h"
#include <type_traits>
#include <memory>

template <typename T>
class basic_cl_client {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

private:
    std::unique_ptr<T> server;
protected:
    virtual std::unique_ptr<T> conn_to_host() = 0;

public:
    void conn_to_server() {
        std::unique_ptr<T> conn = std::move(this->conn_to_host());
        if (conn == nullptr) return;

        server = std::move(conn);
    }

    // this function may be overridden to create encapsulation
    virtual int send_data(void* buff, int count) {
        return ((basic_conn*) server.get())->send_data(buff, count);
    }

    // this function may be overridden to create encapsulation
    virtual int recv_data(void* buff, int count) {
        return ((basic_conn*) server.get())->recv_data(buff, count);
    }

};

#endif //SERVERCLIENT_BASIC_CL_CLIENT_H
