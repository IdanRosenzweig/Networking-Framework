#pragma once

#include "recv_listener.h"
#include "src/utils/circular_queue.h"

#include <vector>
#include <cstdint>
using namespace std;

// receives buff and stores it in a queue
template <typename T>
class recv_queue : public recv_listener<T> {
public:
    circular_queue<T> queue;

    void handle_recv(T const& data) override {
        queue.push_back(data);
    }

};

using recv_queue_bytes = recv_queue<vector<uint8_t>>;
