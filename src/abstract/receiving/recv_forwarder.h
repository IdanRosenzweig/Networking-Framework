#ifndef NETWORKING_RECV_FORWARDER_H
#define NETWORKING_RECV_FORWARDER_H

#include "basic_recv_listener.h"

#include <set>
#include <mutex>
#include <iostream>

using namespace std;

// receives events and forwards them to multiple receivers
template<typename TYPE>
class recv_forwarder : public basic_recv_listener<TYPE> {
    static_assert(is_copy_constructible_v<TYPE>, "type for recv_forwarder must be somehow copy constructable");
    set<basic_recv_listener<TYPE> *> listeners;
    mutex listeners_mtx;

public:
    void add_listener(basic_recv_listener<TYPE> *listener) {
        lock_guard<mutex> lock(listeners_mtx);
        if (listener == nullptr) {
            cerr << "trying to add nullptr as listener" << endl;
            return;
        }
        listeners.insert(listener);
    }

    void clear_listeners() {
        lock_guard<mutex> lock(listeners_mtx);
        listeners.clear();
    }

    void remove_listener(basic_recv_listener<TYPE> *listener) {
        lock_guard<mutex> lock(listeners_mtx);
        if (!listeners.count(listener)) {
            cerr << "trying to remove listener which doesn't exist" << endl;
            return;
        }
        listeners.erase(listener);
    }

    void handle_received_event(TYPE &&event) override {
        lock_guard<mutex> lock(listeners_mtx);
        int no_listeners = listeners.size();
        for (auto &listener: listeners) {
            // create a copy of the event and pass it to each of the sub listeners
            TYPE event_copy(event);
            listener->handle_received_event(std::move(event_copy));
        }
    }

};

#endif //NETWORKING_RECV_FORWARDER_H
