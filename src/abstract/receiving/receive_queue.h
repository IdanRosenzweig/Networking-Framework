#ifndef SERVERCLIENT_RECEIVE_QUEUE_H
#define SERVERCLIENT_RECEIVE_QUEUE_H

#include "basic_receiver.h"
#include "../utils/circular_buffer.h"
#include <queue>

// receives events and stores them in a queue
template <typename T>
class receive_queue : public basic_receiver<T> {
public:
    circular_buffer<T> event_queue;

    void handle_received_event(T&& event) override {
        event_queue.push_back(event);
    }

    T front_data() {
        T front(event_queue.front());
        event_queue.pop_front();
        return front;
    }

};

#endif //SERVERCLIENT_RECEIVE_QUEUE_H
