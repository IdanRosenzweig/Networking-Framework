#ifndef SERVERCLIENT_RECEIVED_MSG_H
#define SERVERCLIENT_RECEIVED_MSG_H

#include "../protocol_t.h"
#include <vector>
#include <memory>
#include <cstring>

struct received_msg {
    std::unique_ptr<uint8_t> data = nullptr;
    int sz = 0;

    std::vector<std::pair<int, protocol_t>> protocol_offsets; // previous offsets of encapsulating protocols
    int curr_offset; // offset to the current data

    received_msg() {}

    received_msg(const received_msg& other) {
        operator=(other);
    }

    received_msg& operator=(const received_msg& other) {
        uint8_t* alloc = new uint8_t[other.sz];
        memcpy(alloc, other.data.get(), other.sz);
        data = std::unique_ptr<uint8_t>(alloc);
        sz = other.sz;

        protocol_offsets = other.protocol_offsets;
        curr_offset = other.curr_offset;

        return *this;
    }

};

#endif //SERVERCLIENT_RECEIVED_MSG_H
