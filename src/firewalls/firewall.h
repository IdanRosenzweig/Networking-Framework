#ifndef SERVERCLIENT_FIREWALL_H
#define SERVERCLIENT_FIREWALL_H

#include <thread>
#include <vector>
#include "../abstract/connection/basic_connection.h"
#include "basic_firewall_filter.h"

class firewall : public basic_connection {
    basic_connection* base_conn;

public:
    std::vector<basic_firewall_filter*> filters;

    firewall(basic_connection* conn) : base_conn(conn) {
        base_conn->add_listener(this);
    }

    int send_data(send_msg val) override {
        return base_conn->send_data(val);
    }

    void handle_received_event(received_msg& event) override {
        for (const auto& filter : filters) {
            switch (filter->calc_policy(event)) {
                case ALLOW: break; // continue to next filter
                case BLOCK: {
                    cout << "firewall blocked packet" << endl;
                    return; // stop
                }
                case DELAY: { // wait and continue
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(1000ms);
                }
            }
        }

        cout << "firewall passed packet" << endl;
        listenable::handle_received_event(event);
    }
};
#endif //SERVERCLIENT_FIREWALL_H
