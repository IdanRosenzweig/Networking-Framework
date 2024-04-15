#ifndef SERVERCLIENT_BASIC_GATEWAY_H
#define SERVERCLIENT_BASIC_GATEWAY_H

#include "basic_send_conn.h"
#include "basic_recv_interrupt_handler.h"

#include <vector>
using namespace std;

class basic_gateway : public basic_send_conn, public basic_recv_interrupt_handler {
    vector<basic_recv_interrupt_handler*> listeners;

public:
    void add_listener(basic_recv_interrupt_handler* listener) {
        listeners.push_back(listener);
    }

    void handle_received_msg(msg_protocol_stack &&msg) override {
        for (auto listener : listeners) {
            listener->handle_received_msg(std::move(msg)); // todo if there is more than 1 listener, this would cause memory err
        }
    }
};

#endif //SERVERCLIENT_BASIC_GATEWAY_H
