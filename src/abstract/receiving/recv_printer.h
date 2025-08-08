#pragma once

#include "recv_listener.h"
#include "src/utils/circular_queue.h"

#include <vector>
#include <cstdint>
#include <iostream>
using namespace std;

// receives buff and stores it in a queue
template <typename T>
class recv_printer : public recv_listener<T> {
public:
    void handle_recv(T const& data) override {
        cout << "printer received" << endl;
    }
};

class recv_printer_bytes : public recv_printer<vector<uint8_t>> {
public:
    void handle_recv(vector<uint8_t> const& data) override {
        cout << "received " << data.size() << " bytes" << endl;
    }
};
