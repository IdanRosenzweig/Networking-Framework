#ifndef NETWORKING_RECV_QUEUE_H
#define NETWORKING_RECV_QUEUE_H

#include "basic_recv_listener.h"
#include "../utils/circular_queue.h"
#include <queue>

// receives buff and stores it in a queue
template <typename T>
class recv_queue : public basic_recv_listener<T> {
public:
    circular_queue<T> queue;

    void handle_callback(T&& data) override {
        queue.push_back(data);
    }

};

#endif //NETWORKING_RECV_QUEUE_H
