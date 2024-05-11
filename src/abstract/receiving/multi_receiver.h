#ifndef SERVERCLIENT_MULTI_RECEIVER_H
#define SERVERCLIENT_MULTI_RECEIVER_H

#include "basic_receiver.h"
#include <vector>

// multiplexes many receivers as one
template <typename T>
class multi_receiver : public basic_receiver<T> {
    std::vector<basic_receiver<T>*> listeners;

public:
    void add_listener(basic_receiver<T>* listener) {
        listeners.push_back(listener);
    }

    void handle_received_event(T& event) override {
        for (auto listener : listeners) {
            listener->handle_received_event(event);
        }
    }

};

#endif //SERVERCLIENT_MULTI_RECEIVER_H
