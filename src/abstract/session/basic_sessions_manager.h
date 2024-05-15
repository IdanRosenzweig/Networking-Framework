#ifndef SERVERCLIENT_BASIC_SESSIONS_MANAGER_H
#define SERVERCLIENT_BASIC_SESSIONS_MANAGER_H

#include "basic_session.h"
#include "basic_session_generator.h"
#include "../receiving/msg/msg_receiver.h"
#include "basic_session_handler.h"
#include <memory>
#include <vector>

// class that takes a session generator and manages its sessions.
// creates a session handler for each new session and instantiates it
template<typename SESSION_TYPE, typename SESSION_HANDLER>
class basic_sessions_manager {
//    static_assert(std::is_base_of_v<msg_session, SESSION_TYPE>,
//                  "template for basic_sessions_manager session_type must be derived type of basic_session");
    static_assert(std::is_base_of_v<basic_session_handler<SESSION_TYPE>, SESSION_HANDLER>,
                  "template for basic_sessions_manager app_handler must be derived type of basic_session_handler");

    struct internal_session_handler {
        std::unique_ptr<SESSION_TYPE> session;
        SESSION_HANDLER handler;

    public:
        explicit internal_session_handler(std::unique_ptr<SESSION_TYPE> &&sess)
                : session(std::move(sess)), handler(session) {
            handler.session_started();
        }
    };

    struct sessions_recv : basic_receiver<std::unique_ptr<SESSION_TYPE>> {
        basic_sessions_manager *master;

        void handle_received_event(std::unique_ptr<SESSION_TYPE> &event) override {
            master->sessions.emplace_back(std::move(event));
        }

        explicit sessions_recv(basic_sessions_manager *master) : master(master) {}
    };

    sessions_recv receiver;

public:
    std::vector<internal_session_handler> sessions;

    basic_sessions_manager() = delete;

    basic_sessions_manager(basic_session_generator<SESSION_TYPE> *sessions_generator) : receiver(this) {
        sessions_generator->add_listener(&receiver);
    }

};

#endif //SERVERCLIENT_BASIC_SESSIONS_MANAGER_H
