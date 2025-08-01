#pragma once

#include "src/utils/circular_queue.h"
#include "src/abstract/receiving/basic_recv_listener.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
using namespace std;

// receives data and forwards it async to a receiver
template<typename TYPE>
class async_recv_listener : public basic_recv_listener<TYPE>  {
    // todo lock that prioritizes the worker
    shared_ptr<basic_recv_listener<TYPE>> listener;
    mutex listener_mtx;

    // todo lock that prioritizes the worker
    circular_queue<TYPE> queue;
    mutex buff_mtx;

    thread worker;
    mutex awake_mtx;
    atomic<bool> stop_flag;
    condition_variable wake_condition;

public:
    void set_recv_listener(shared_ptr<basic_recv_listener<TYPE>> const& listener) {
        if (listener == nullptr) return;

        lock_guard<mutex> lock(listener_mtx);
        this->listener = listener;
    }

    void remove_listener() {
        lock_guard<mutex> lock(listener_mtx);
        listener = nullptr;
    }

    async_recv_listener() {
        stop_flag = false;
        worker = thread([&]() -> void {
            while (!stop_flag) {
                std::unique_lock<std::mutex> wake_lock(this->awake_mtx);
                wake_condition.wait(wake_lock, [&] {
                    if (stop_flag) return true;
                    lock_guard<mutex> lock(this->buff_mtx);
                    return !this->queue.is_empty();
                });
                if (stop_flag) break;

                // take data
                lock_guard<mutex> lock_q(buff_mtx);
                if (queue.is_empty()) continue;
                TYPE data = *queue.front(std::chrono::milliseconds(0));
                queue.pop_front();

                // forward to listener
                lock_guard<mutex> lock_listener(listener_mtx);
                if (listener == nullptr) continue;
                listener->handle_recv(data);
            }
        });
    }

    ~async_recv_listener() {
        stop_flag = true;
        wake_condition.notify_all();
        worker.detach();
    }

    void handle_recv(TYPE const& data) override {
        lock_guard<mutex> lock(buff_mtx);

        if (queue.is_full()) return;
        queue.push_back(data);
        wake_condition.notify_one();
    }

};