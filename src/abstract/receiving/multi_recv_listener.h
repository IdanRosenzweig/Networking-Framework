#pragma once

#include "basic_recv_listener.h"

#include <set>
#include <mutex>
#include <iostream>
#include <memory>
using namespace std;

// receives data and forwards it to multiple receivers
template<typename TYPE>
class multi_recv_listener : public basic_recv_listener<TYPE> {
    set<shared_ptr<basic_recv_listener<TYPE>>> listeners;
    mutex listeners_mtx;

public:
    void add_recv_listener(shared_ptr<basic_recv_listener<TYPE>> const& listener) {
        if (listener == nullptr) return;

        lock_guard<mutex> lock(listeners_mtx);
        listeners.insert(listener);
    }

    void remove_all_recv_listeners() {
        lock_guard<mutex> lock(listeners_mtx);
        listeners.clear();
    }

    bool remove_listener(shared_ptr<basic_recv_listener<TYPE>> const& listener) {
        if (listener == nullptr) return false;

        lock_guard<mutex> lock(listeners_mtx);

        if (!listeners.count(listener)) {
            return false;
        }

        listeners.erase(listener);
        return true;
    }

    void handle_recv(TYPE const& data) override {
        vector<shared_ptr<basic_recv_listener<TYPE>>> listeners_copy;
        {
            lock_guard<mutex> lock(listeners_mtx);
            for (auto const& listener : listeners) listeners_copy.push_back(listener);
        }

        for (auto& listener: listeners) {
            listener->handle_recv(data);
        }
    }

};
