#ifndef SERVERCLIENT_SESSION_GENERATOR_H
#define SERVERCLIENT_SESSION_GENERATOR_H

#include "../receiving/basic_recv_listener.h"
#include "session_t.h"

// object that generates new sessions and forwards them to a listener
template <typename SESSION_TYPE>
class session_generator {
    basic_recv_listener<SESSION_TYPE>* listener = nullptr;

public:
    void set_generator_listener(basic_recv_listener<SESSION_TYPE>* handler) {
        listener = handler;
    }

    void generate_event(SESSION_TYPE&& event) {
        if (listener != nullptr)
            listener->handle_received_event(std::move(event));
    }
};


#endif //SERVERCLIENT_SESSION_GENERATOR_H
