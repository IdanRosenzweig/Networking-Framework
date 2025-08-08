#pragma once

#include "circular_queue.h"
#include "src/abstract/receiving/recv_listener.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
using namespace std;





// template<typename T>
// class network_atomic_queue : public muilti_recv_listener<T> {
//     circular_queue<T> queue;
//     mutex buff_mtx;

//     thread worker;
//     mutex awake_mtx;
//     atomic<bool> stop_flag;
//     condition_variable wake_condition;

// public:
//     void add_msg(T &&msg) {
//         lock_guard<mutex> lock(buff_mtx);

//         if (queue.is_full()) return;
//         queue.push_back(std::move(msg));
//         wake_condition.notify_one();
//     }

//     network_atomic_queue() {
//         stop_flag = false;
//         worker = thread([&]() -> void {
//             while (!stop_flag) {
//                 std::unique_lock<std::mutex> wake_lock(this->awake_mtx);
//                 wake_condition.wait(wake_lock, [&] {
//                     if (stop_flag) return true;
//                     lock_guard<mutex> lock(this->buff_mtx);
//                     return !this->queue.is_empty();
//                 });
//                 if (stop_flag) break;

//                 lock_guard<mutex> lock(buff_mtx);

//                 if (queue.is_empty()) continue;
//                 T msg = *queue.front();
//                 queue.pop_front();
//                 this->handle_callback(std::move(msg));
//             }
//         });
//     }

//     ~network_atomic_queue() {
//         stop_flag = true;
//         wake_condition.notify_all();
//         worker.detach();
//     }
// };

