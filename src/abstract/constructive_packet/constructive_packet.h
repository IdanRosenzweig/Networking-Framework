#pragma once

#include <stdint.h>
#include <cstring>
#include <deque>
using namespace std;

class constructive_packet {
    deque<uint8_t> buff;

public:
    deque<uint8_t> const& get_buff() {
        return buff;
    }

    void clear_buff() {
        buff.clear();
    }

    void append_data(uint8_t const* data, size_t sz) {
        for (size_t off = 0; off < sz; off++)
            buff.push_back(data[off]);
    }

    void prepend_data(uint8_t const* data, size_t sz) {
        for (size_t off = 0; off < sz; off++)
            buff.push_front(data[off]);
    }

    // insert the data inside the packet at specific offset
    void encapsulate_inside_packet(uint8_t const* data, size_t sz, size_t offset) {
        buff.insert(buff.begin() + offset, data, data + sz);
    }

    // insert the current packet at specific offset inside the data, and store the result to the packet back
    void encapsulate_the_packet(uint8_t const* data, size_t sz, size_t offset) {
        buff.insert(buff.begin(), data, data + offset);
        buff.insert(--buff.end(), data + offset, data + sz);
    }
};