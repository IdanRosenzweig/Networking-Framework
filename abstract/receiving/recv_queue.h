#ifndef SERVERCLIENT_RECV_QUEUE_H
#define SERVERCLIENT_RECV_QUEUE_H

#include "basic_receiver.h"
#include <queue>
#include <thread>

// basic use of receiving interrupted data: storing them in a queue
template <typename T>
class recv_queue : public basic_receiver<T> {
private:
    std::queue<T> income_queue;

public:
    void handle_received_event(T event) override {
        income_queue.push(event);
    }

    T front_data() {
        while (income_queue.empty()) { // polling, todo this can maybe be improved
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
        }

        T front = income_queue.front();
        income_queue.pop();
        return front;
    }

};

#endif //SERVERCLIENT_RECV_QUEUE_H
