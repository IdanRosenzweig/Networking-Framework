#ifndef SERVERCLIENT_NETWORK_MSG_QUEUE_H
#define SERVERCLIENT_NETWORK_MSG_QUEUE_H

#include "circular_buffer.h"
#include "../receiving/msg/received_msg.h"
#include "../receiving/recv_forwarder.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

// a queue for storing messages for farther receiving/sending.
// message and stored in a circular queue.
// add message through add_msg
// when the queue is not empty, a thread worker will pop messages and use the recv_forwarder api to forward them

template<typename T, int MAX_NO_MSG = 50000>
class network_msg_queue : public recv_forwarder<T> {
    circular_buffer<T, MAX_NO_MSG> ring_buffer;
    mutex buff_mtx;

    thread worker;
    mutex awake_mtx;
    atomic<bool> stop_flag;
    condition_variable wake_condition;

public:
    void add_msg(T &&msg) {
        lock_guard<mutex> lock(buff_mtx);

        if (ring_buffer.is_full()) return;
        ring_buffer.push_back(std::move(msg));
        wake_condition.notify_one();
    }

    network_msg_queue() {
        stop_flag = false;
        worker = thread([&]() -> void {
            while (!stop_flag) {
                std::unique_lock<std::mutex> wake_lock(this->awake_mtx);
                wake_condition.wait(wake_lock, [&] {
                    if (stop_flag) return true;
                    lock_guard<mutex> lock(this->buff_mtx);
                    return !this->ring_buffer.is_empty();
                });
                if (stop_flag) break;

                lock_guard<mutex> lock(buff_mtx);

                if (ring_buffer.is_empty()) continue;
                T msg = *ring_buffer.front();
                ring_buffer.pop_front();
                this->handle_received_event(std::move(msg));
            }
        });
    }

    ~network_msg_queue() {
        stop_flag = true;
        wake_condition.notify_all();
        worker.detach();
    }
};


#endif //SERVERCLIENT_NETWORK_MSG_QUEUE_H
