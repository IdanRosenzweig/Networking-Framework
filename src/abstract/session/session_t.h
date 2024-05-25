#ifndef SERVERCLIENT_SESSION_T_H
#define SERVERCLIENT_SESSION_T_H

#include <memory>

template <typename SESSION_CONN, typename SESSION_DATA>
struct session_t {
    static_assert(std::is_base_of_v<session_conn, SESSION_CONN>, "type of SESSION_CONN must be derived class of session_conn");

    SESSION_DATA sess_data;
    std::unique_ptr<SESSION_CONN> sess_conn;

    session_t() : sess_conn(nullptr) {}
    session_t(const SESSION_DATA& data, std::unique_ptr<SESSION_CONN> &&session) : sess_data(data), sess_conn(std::move(session)) {}

    session_t(const session_t& other) = delete;
    session_t& operator=(const session_t& other) = delete;

    session_t(session_t&& other) : sess_data(std::move(other.sess_data)), sess_conn(std::move(other.sess_conn)) {
    }
    session_t& operator=(session_t&& other) {
        sess_data = std::move(other.sess_data);
        sess_conn.reset(other.sess_conn.release());

        return *this;
    }

};


#endif //SERVERCLIENT_SESSION_T_H
