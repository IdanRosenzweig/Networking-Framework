#ifndef SERVERCLIENT_MULTI_SENDER_H
#define SERVERCLIENT_MULTI_SENDER_H

#include "basic_sender.h"
#include <vector>

template <typename T>
class multi_sender : public basic_sender<T> {
    std::vector<basic_sender<T>*> senders;

public:
    void add_sender(basic_sender<T>* sender) {
        senders.push_back(sender);
    }

    int send_data(T val) override {
        for (auto sender : senders) {
            sender->send_data(val);
        }
    }

};
#endif //SERVERCLIENT_MULTI_SENDER_H
