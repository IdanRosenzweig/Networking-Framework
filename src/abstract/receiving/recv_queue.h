#ifndef NETWORKING_RECV_QUEUE_H
#define NETWORKING_RECV_QUEUE_H

#include "basic_recv_listener.h"
#include "../utils/circular_buffer.h"
#include <queue>

// receives data and stores it in a queue
template <typename T>
class recv_queue : public basic_recv_listener<T> {
public:
    circular_buffer<T> recv_queue;

    void handle_received_event(T&& event) override {
        recv_queue.push_back(event);
    }

};

#endif //NETWORKING_RECV_QUEUE_H
