#pragma once

#include "send_medium.h"

#include <set>
#include <mutex>
#include <memory>
#include <iostream>
#include <cstdint>
#include <vector>
using namespace std;

// sends to multiple send_mediums
template <typename TYPE>
class multi_send_medium : public send_medium<TYPE> {
    set<shared_ptr<send_medium<TYPE>>> mediums;
    mutex mediums_mtx;

public:
    void add_send_medium(shared_ptr<send_medium<TYPE>> const& channel) {
        if (channel == nullptr) return;

        lock_guard<mutex> lock(mediums_mtx);
        mediums.insert(channel);
    }
    
    void remove_all_send_mediums() {
        lock_guard<mutex> lock(mediums_mtx);
        mediums.clear();
    }

    bool remove_send_medium(shared_ptr<send_medium<TYPE>> const& channel) {
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

using multi_send_medium_bytes = multi_send_medium<vector<uint8_t>>;
