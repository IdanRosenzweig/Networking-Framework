#ifndef SERVERCLIENT_RECEIVE_MULTIPLEXER_H
#define SERVERCLIENT_RECEIVE_MULTIPLEXER_H

#include "basic_receiver.h"
#include <set>

#include <iostream>
using namespace std;

// receives events and multiplexes them to multiple receivers
template <typename TYPE>
class receive_multiplexer : public basic_receiver<TYPE> {
    static_assert(std::is_copy_constructible_v<TYPE>, "type for receive_multiplexer must be somehow copy constructable");
    std::set<basic_receiver<TYPE>*> listeners;

public:
    void add_listener(basic_receiver<TYPE>* listener) {
        listeners.insert(listener);
    }
    void clear_listeners() {listeners.clear();}
    void remove_listener(basic_receiver<TYPE>* listener) {
        if (!listeners.count(listener)) {
            std::cerr << "trying to remove listener which doesn't exist" << endl;
            return;
        }
        listeners.erase(listener);
    }

    void handle_received_event(TYPE&& event) override {

        if (listeners.size() == 2) {
            volatile int a= 0;
        }
        for (auto& listener : listeners) {
            // create a copy of the event and pass it to each of the sub listeners
            TYPE event_copy(event);
            listener->handle_received_event(std::move(event_copy));
        }
    }

};

#endif //SERVERCLIENT_RECEIVE_MULTIPLEXER_H
