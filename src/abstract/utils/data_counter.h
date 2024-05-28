#ifndef NETWORKING_DATA_COUNTER_H
#define NETWORKING_DATA_COUNTER_H

#include "../receiving/msg/received_msg.h"

class data_counter : public msg_recv_listener {
    int counter = 0;
public:
    int get_count() {return counter;}
    void reset() {counter = 0;}
    void handle_received_event(received_msg&& event) override {
        counter++;
    }
};

#endif //NETWORKING_DATA_COUNTER_H
