#ifndef SERVERCLIENT_SEND_FORWARDER_H
#define SERVERCLIENT_SEND_FORWARDER_H

#include "basic_sender.h"
#include <vector>

template <typename TYPE>
class send_forwarder : public basic_sender<TYPE> {
    std::vector<basic_sender<TYPE>*> targets;

public:
    void add_send_target(basic_sender<TYPE>* sender) {
        targets.push_back(sender);
    }

    int send_data(TYPE&& val) override {
        int sum = 0;

        for (auto target : targets) {
            // create a copy pass it to each of the targets
            TYPE val_copy(val);
            sum += target->send_data(std::move(val_copy));
        }

        return sum;
    }

};

#endif //SERVERCLIENT_SEND_FORWARDER_H
