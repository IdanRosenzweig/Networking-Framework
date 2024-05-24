#ifndef SERVERCLIENT_RECEIVE_QUEUE_H
#define SERVERCLIENT_RECEIVE_QUEUE_H

#include "basic_receiver.h"
#include "../utils/circular_buffer.h"
#include <queue>

// receives events and stores them in a queue
template <typename T>
class receive_queue : public basic_receiver<T> {
public:
    circular_buffer<T> recv_queue;

    void handle_received_event(T&& event) override {
        recv_queue.push_back(event);
    }

};

#endif //SERVERCLIENT_RECEIVE_QUEUE_H
