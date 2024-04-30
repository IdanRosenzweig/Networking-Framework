#ifndef SERVERCLIENT_DATA_COUNTER_H
#define SERVERCLIENT_DATA_COUNTER_H

#include "../receiving/msg_receiver.h"

class data_counter : public msg_receiver {
    int counter = 0;
public:
    int get_count() {return counter;}
    void reset() {counter = 0;}
    void handle_received_event(received_msg& event) override {
        counter++;
    }
};

#endif //SERVERCLIENT_DATA_COUNTER_H
