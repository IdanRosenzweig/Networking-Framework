#pragma once

#include <cstdint>
#include <vector>
using namespace std;

// receiving data is often different from sending data in the sense that
// you can control when to send data, but you don't know when you will receive data.

template <typename T>
class recv_listener {
public:
    // this callback function will be called when data is received
    virtual void handle_recv(T const& data) = 0;

    virtual ~recv_listener() = default;
};

using recv_listener_bytes = recv_listener<vector<uint8_t>>;
