#ifndef SERVERCLIENT_BASIC_RECV_CONN_H
#define SERVERCLIENT_BASIC_RECV_CONN_H

#include "basic_recv_interrupt_handler.h"
#include <queue>
#include <thread>

// this class creates the illusion of controlling when data is received
// by creating a hidden wait queue for interrupted data
class basic_recv_conn : public basic_recv_interrupt_handler {
private:
    std::queue<msg_protocol_stack> income_queue;

    void handle_received_msg(msg_protocol_stack &&msg) override {
        income_queue.push(std::move(msg));
    }

public:
    msg_protocol_stack recv_data() {
        while (income_queue.empty()) { // polling, todo this can maybe be improved
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);
        }

        msg_protocol_stack front = std::move(income_queue.front());
        income_queue.pop();
        return std::move(front);
    }

};

#endif //SERVERCLIENT_BASIC_RECV_CONN_H
