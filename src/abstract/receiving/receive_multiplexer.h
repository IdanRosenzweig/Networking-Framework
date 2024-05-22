#ifndef SERVERCLIENT_RECEIVE_MULTIPLEXER_H
#define SERVERCLIENT_RECEIVE_MULTIPLEXER_H

#include "basic_receiver.h"
#include <vector>

// receives events and multiplexes them to multiple receivers
template <typename TYPE>
class receive_multiplexer : public basic_receiver<TYPE> {
    static_assert(std::is_copy_constructible_v<TYPE>, "type for receive_multiplexer must be somehow copy constructable");
    std::vector<basic_receiver<TYPE>*> listeners;

public:
    void add_listener(basic_receiver<TYPE>* listener) {
        listeners.push_back(listener);
    }

    void handle_received_event(TYPE&& event) override {

        for (auto listener : listeners) {
            // create a copy of the event and pass it to each of the sub listeners
            TYPE event_copy(event);
            listener->handle_received_event(std::move(event_copy));
        }
    }

};

#endif //SERVERCLIENT_RECEIVE_MULTIPLEXER_H
