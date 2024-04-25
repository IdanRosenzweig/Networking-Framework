#ifndef SERVERCLIENT_LISTENABLE_H
#define SERVERCLIENT_LISTENABLE_H

#include "basic_receiver.h"
#include <vector>

// basic use of receiving interrupted data: forwarding them to sub receivers
template <typename T>
class listenable : public basic_receiver<T> {
    std::vector<basic_receiver<T>*> listeners;

public:
    void add_listener(basic_receiver<T>* listener) {
        listeners.push_back(listener);
    }

    void handle_received_event(T event) override {
        for (auto listener : listeners) {
            listener->handle_received_event(event);
        }
    }

};

#endif //SERVERCLIENT_LISTENABLE_H
