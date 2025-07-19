#pragma once

#include "basic_recv_listener.h"
#include "src/utils/circular_queue.h"

// receives buff and stores it in a queue
template <typename T>
class recv_queue : public basic_recv_listener<T> {
public:
    circular_queue<T> queue;

    void handle_recv(T const& data) override {
        queue.push_back(data);
    }

};
