#ifndef NETWORKING_SESSION_GENERATOR_H
#define NETWORKING_SESSION_GENERATOR_H

#include "../receiving/recv_listener.h"
#include "session_t.h"

// object that generates new sessions and forwards them to a listener
template <typename SESSION_TYPE>
class session_generator {
    recv_listener<SESSION_TYPE>* listener = nullptr;

public:
    void set_generator_listener(recv_listener<SESSION_TYPE>* handler) {
        listener = handler;
    }

    void generate_session(SESSION_TYPE&& session) {
        if (listener != nullptr)
            listener->handle_callback(std::move(session));
    }
};


#endif //NETWORKING_SESSION_GENERATOR_H
