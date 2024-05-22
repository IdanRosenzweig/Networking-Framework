#ifndef SERVERCLIENT_SESSION_T_H
#define SERVERCLIENT_SESSION_T_H

#include <memory>

template <typename SESSION_CONN>
struct session_t {
    static_assert(std::is_base_of_v<session_conn, SESSION_CONN>, "type of SESSION_CONN must be derived class of session_conn");

    int id;
    std::unique_ptr<SESSION_CONN> session;

    session_t() : id(0), session(nullptr) {}
    session_t(int id, std::unique_ptr<SESSION_CONN> &&session) : id(id), session(std::move(session)) {}

};


#endif //SERVERCLIENT_SESSION_T_H
