#ifndef SERVERCLIENT_SEND_FORWARDER_H
#define SERVERCLIENT_SEND_FORWARDER_H

#include "basic_send_medium.h"
#include <vector>
#include <mutex>
#include <iostream>
using namespace std;

// forwards data along multiple mediums
template <typename TYPE>
class send_forwarder : public basic_send_medium<TYPE> {
    std::vector<basic_send_medium<TYPE>*> mediums;
    mutex mediums_mtx;

public:
    void add_send_medium(basic_send_medium<TYPE>* medium) {
        lock_guard<mutex> lock(mediums_mtx);
        mediums.push_back(medium);
    }
    void clear_send_mediums() {
        lock_guard<mutex> lock(mediums_mtx);
        mediums.clear();
    }
    void remove_send_medium(basic_send_medium<TYPE>* medium) {
        lock_guard<mutex> lock(mediums_mtx);
        if (!mediums.count(medium)) {
            cerr << "trying to remove listener which doesn't exist" << endl;
            return;
        }
        mediums.erase(medium);
    }

    int send_data(TYPE&& val) override {
        int sum = 0;

        for (auto& target : mediums) {
            // create a copy pass it to each of the mediums
            TYPE val_copy(val);
            sum += target->send_data(std::move(val_copy));
        }

        return sum;
    }

};

#endif //SERVERCLIENT_SEND_FORWARDER_H
