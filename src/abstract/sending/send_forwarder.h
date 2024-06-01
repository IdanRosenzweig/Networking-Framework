#ifndef NETWORKING_SEND_FORWARDER_H
#define NETWORKING_SEND_FORWARDER_H

#include "basic_send_medium.h"
#include <vector>
#include <mutex>
#include <iostream>
using namespace std;

// forwards buff along multiple mediums
template <typename TYPE>
class send_forwarder : public basic_send_medium<TYPE> {
    std::vector<basic_send_medium<TYPE>*> mediums;
    mutex mediums_mtx;

public:
    void add_send_channel(basic_send_medium<TYPE>* channel) {
        lock_guard<mutex> lock(mediums_mtx);
        mediums.push_back(channel);
    }
    void clear_send_channels() {
        lock_guard<mutex> lock(mediums_mtx);
        mediums.clear();
    }
    void remove_send_channel(basic_send_medium<TYPE>* channel) {
        lock_guard<mutex> lock(mediums_mtx);
        if (!mediums.count(channel)) {
            cerr << "trying to remove send chanell which doesn't exist" << endl;
            return;
        }
        mediums.erase(channel);
    }

    int send_data(TYPE&& data) override {
        int sum = 0;

        for (auto& target : mediums) {
            // create a copy pass it to each of the channels
            TYPE data_copy(data);
            sum += target->send_data(std::move(data));
        }

        return sum;
    }

};

#endif //NETWORKING_SEND_FORWARDER_H
