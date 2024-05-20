#ifndef SERVERCLIENT_RECEIVED_MSG_H
#define SERVERCLIENT_RECEIVED_MSG_H

#include "../../protocol_t.h"
#include "../../utils/udata.h"
#include <vector>
#include <memory>
#include <cstring>

struct received_msg {
    udata_t data;

    std::vector<std::pair<int, protocol_t>> protocol_offsets; // previous offsets of encapsulating protocols
    int curr_offset; // offset to the current msg

    received_msg() {}

    received_msg(const received_msg& other) {
        operator=(other);
    }

    received_msg& operator=(const received_msg& other) {
        int sz = other.data.size();
        data = udata_t(sz, 0x00);
        memcpy(data.data(), other.data.data(), sz);

        curr_offset = 0;

        protocol_offsets = other.protocol_offsets;
        curr_offset = other.curr_offset;

        return *this;
    }

};

#endif //SERVERCLIENT_RECEIVED_MSG_H
