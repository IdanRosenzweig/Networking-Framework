#ifndef SERVERCLIENT_RECV_QUEUE_H
#define SERVERCLIENT_RECV_QUEUE_H

#include "basic_receiver.h"
#include "../utils/circular_buffer.h"
#include <queue>
#include <thread>

// basic use of receiving interrupted msg: storing them in a queue
template <typename T>
class recv_queue : public basic_receiver<T> {
public:
    circular_buffer<T> income_queue;

    void handle_received_event(T& event) override {
        income_queue.push_back(event);
    }

    T front_data() {
        T front = income_queue.front();
        income_queue.pop_front();
        return front;
    }

};

#endif //SERVERCLIENT_RECV_QUEUE_H
