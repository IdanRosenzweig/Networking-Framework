#ifndef NETWORKING_DATA_COUNTER_H
#define NETWORKING_DATA_COUNTER_H

#include "../receiving/msg/recv_msg_t.h"

class data_counter : public msg_recv_listener {
    int counter = 0;
public:
    int get_count() {return counter;}
    void reset() {counter = 0;}
    void handle_callback(recv_msg_t&& data) override {
        counter++;
    }
};

#endif //NETWORKING_DATA_COUNTER_H
