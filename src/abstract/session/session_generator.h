#ifndef SERVERCLIENT_SESSION_GENERATOR_H
#define SERVERCLIENT_SESSION_GENERATOR_H

#include "../receiving/basic_receiver.h"
#include "session_t.h"

// object that generates new sessions and forwards them to a listener
template <typename SESSION_CONN>
class session_generator {
    basic_receiver<session_t<SESSION_CONN>>* listener = nullptr;

public:
    void set_generator_listener(basic_receiver<session_t<SESSION_CONN>>* handler) {
        listener = handler;
    }

    void generate_event(session_t<SESSION_CONN>&& event) {
        if (listener != nullptr)
            listener->handle_received_event(std::move(event));
    }
};


#endif //SERVERCLIENT_SESSION_GENERATOR_H
