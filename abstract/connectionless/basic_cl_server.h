#ifndef SERVERCLIENT_BASIC_CL_SERVER_H
#define SERVERCLIENT_BASIC_CL_SERVER_H


#include "../basic_conn.h"
#include <type_traits>
#include <queue>
#include <memory>


template <typename T>
class basic_cl_server {
    static_assert(std::is_base_of<basic_conn, T>(), "connection type must inherit basic_conn");

protected:
    // discover next host trying to send us data
    virtual std::unique_ptr<T> discover_next_host() = 0;

public:
    std::queue<std::unique_ptr<basic_conn>> clients_q;

    void discover_next_client() {
        std::unique_ptr<T> conn = std::move(this->discover_next_host());
        if (conn == nullptr) return;

        clients_q.push(std::move(conn));
    }

};

#endif //SERVERCLIENT_BASIC_CL_SERVER_H
