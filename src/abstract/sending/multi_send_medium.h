#pragma once

#include "basic_send_medium.h"

#include <set>
#include <mutex>
#include <memory>
#include <iostream>
using namespace std;

// sends to multiple basic_send_mediums
template <typename TYPE>
class multi_send_medium : public basic_send_medium<TYPE> {
    set<shared_ptr<basic_send_medium<TYPE>>> mediums;
    mutex mediums_mtx;

public:
    void add_send_medium(shared_ptr<basic_send_medium<TYPE>> const& channel) {
        if (channel == nullptr) return;

        lock_guard<mutex> lock(mediums_mtx);
        mediums.insert(channel);
    }
    
    void remove_all_send_mediums() {
        lock_guard<mutex> lock(mediums_mtx);
        mediums.clear();
    }

    bool remove_send_medium(shared_ptr<basic_send_medium<TYPE>> const& channel) {
        if (channel == nullptr) return;

        lock_guard<mutex> lock(mediums_mtx);

        if (!mediums.count(channel)) {
            return false;
        }

        mediums.erase(channel);
        return true;
    }

    err_t send_data(TYPE const& data) override {
        lock_guard<mutex> lock(mediums_mtx);

        err_t res = err_t::OK;

        for (auto& medium : mediums) {
            if (medium->send_data(data) == err_t::ERR)
                res = err_t::ERR;
        }

        return res;
    }

};

