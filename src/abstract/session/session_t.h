#ifndef SERVERCLIENT_SESSION_T_H
#define SERVERCLIENT_SESSION_T_H

#include <memory>

template <typename SESSION_CONN, typename SESSION_DATA>
struct session_t {
    static_assert(std::is_base_of_v<session_conn, SESSION_CONN>, "type of SESSION_CONN must be derived class of session_conn");

    int id;
    SESSION_DATA sess_data;
    std::unique_ptr<SESSION_CONN> sess_conn;

    session_t() : id(0), sess_conn(nullptr) {}
    session_t(int id, const SESSION_DATA data, std::unique_ptr<SESSION_CONN> &&session) : id(id), sess_data(data), sess_conn(std::move(session)) {}

};


#endif //SERVERCLIENT_SESSION_T_H
