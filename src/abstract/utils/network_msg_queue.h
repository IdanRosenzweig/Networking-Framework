#ifndef SERVERCLIENT_NETWORK_MSG_QUEUE_H
#define SERVERCLIENT_NETWORK_MSG_QUEUE_H

#include "circular_buffer.h"
#include "../receiving/msg/received_msg.h"
#include "../receiving/receive_forwarder.h"

#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

// a queue for storing message for farther receiving/sending.
template <typename T, int MAX_NO_MSG = 50000>
class network_msg_queue : public receive_forwarder<T> {
    circular_buffer<T, MAX_NO_MSG> ring_buffer;
    mutex buff_mtx;

    thread worker;
    mutex awake_mtx;
    condition_variable wake_condition;

protected:
    void add_msg(T&& msg) {
        static mutex internal_mtx;
        {
            lock_guard<mutex> lock(internal_mtx);
        }

        lock_guard<mutex> lock(buff_mtx);

        if (ring_buffer.is_full()) return;
        ring_buffer.push_back(std::move(msg));
        wake_condition.notify_one();
    }

public:
    network_msg_queue() {
        worker = thread([&]() -> void {
            while (true) {
                std::unique_lock<std::mutex> wake_lock(this->mtx);
                wake_condition.wait(wake_lock, [&]{ lock_guard<mutex> lock(this->buff_mtx); return !this->ring_buffer.is_empty(); });

                lock_guard<mutex> lock(buff_mtx);

                if (ring_buffer.is_empty()) continue;
                T msg = *ring_buffer.front();
                ring_buffer.pop_front();
                this->handle_received_event(std::move(msg));
            }
        });
    }

    ~network_msg_queue() {
        if (worker.joinable())
            worker.detach();
    }
};


#endif //SERVERCLIENT_NETWORK_MSG_QUEUE_H
