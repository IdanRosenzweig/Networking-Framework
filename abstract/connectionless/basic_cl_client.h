#ifndef SERVERCLIENT_BASIC_CL_CLIENT_H
#define SERVERCLIENT_BASIC_CL_CLIENT_H

#include "../basic_conn.h"
#include <type_traits>
#include <memory>

template <typename T>
class basic_cl_client {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

protected:
    virtual std::unique_ptr<T> conn_to_host() = 0;

public:
    std::unique_ptr<T> server;

    void conn_to_server() {
        std::unique_ptr<T> conn = std::move(this->conn_to_host());
        if (conn == nullptr) return;

        server = std::move(conn);
    }
};

#endif //SERVERCLIENT_BASIC_CL_CLIENT_H
