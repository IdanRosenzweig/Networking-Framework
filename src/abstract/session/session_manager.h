#ifndef SERVERCLIENT_SESSION_MANAGER_H
#define SERVERCLIENT_SESSION_MANAGER_H

#include "session_conn.h"
#include "session_generator.h"
#include "../receiving/msg/received_msg.h"
#include "session_handler.h"
#include <memory>
#include <vector>

// class that takes a session generator and manages all its generated sessions.
// creates a session handler for each new session and instantiates it
template<typename SESSION_TYPE, typename SESSION_HANDLER>
class session_manager {
    static_assert(std::is_base_of_v<session_handler<SESSION_TYPE>, SESSION_HANDLER>,
                  "type of SESSION_HANDLER must be derived type of session_handler<SESSION_TYPE>");

    struct internal_session_handler {
        SESSION_HANDLER handler;
        int internal_id;

    public:
        explicit internal_session_handler(SESSION_TYPE &&sess, int id)
                : handler(std::move(sess)), internal_id(id) {
            handler.on_session_start();
        }

        internal_session_handler(internal_session_handler &&other) : handler(std::move(other.handler.session)),
                                                                     internal_id(other.internal_id) {

        }

        internal_session_handler &operator=(internal_session_handler &&other) {
            handler = std::move(other.handler.session);
            internal_id = other.internal_id;
        }

        bool operator<(const internal_session_handler &rhs) const {
            return internal_id < rhs.internal_id;
        }

        bool operator>(const internal_session_handler &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const internal_session_handler &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const internal_session_handler &rhs) const {
            return !(*this < rhs);
        }
    };

    struct sessions_recv : basic_recv_listener<SESSION_TYPE> {
        session_manager *master;

        void handle_received_event(SESSION_TYPE &&event) override {
            master->sessions.insert(std::make_unique<internal_session_handler>(std::move(event), master->next_id++));
        }

        explicit sessions_recv(session_manager *master) : master(master) {}
    };

    sessions_recv receiver;

    int next_id = 0;
public:
    std::set<std::unique_ptr<internal_session_handler>> sessions;

    session_manager() = delete;

    session_manager(session_generator<SESSION_TYPE> *sessions_generator) : receiver(this) {
        sessions_generator->set_generator_listener(&receiver);
    }

};

#endif //SERVERCLIENT_SESSION_MANAGER_H
