#ifndef SERVERCLIENT_RECEIVED_MSG_H
#define SERVERCLIENT_RECEIVED_MSG_H

#include "../../utils/udata.h"
#include "../../protocol_t.h"
#include "../basic_recv_listener.h"

#include <vector>
#include <cstring>
using namespace std;

struct received_msg {
    udata_t data;

    std::vector<std::pair<int, protocol_t>> protocol_offsets; // previous offsets of encapsulating protocols
    int curr_offset; // offset to the current msg

    received_msg() : data(), protocol_offsets(), curr_offset(0) {}

    received_msg(const received_msg& other) {
        int cnt = other.data.size();
        data = udata_t(cnt, 0);
        memcpy(data.data(), other.data.c_str(), cnt);

        protocol_offsets = other.protocol_offsets;
        curr_offset = other.curr_offset;
    }

//    received_msg(received_msg&& other) {
//        data = other.data;
//        other.data.clear();
//
//        protocol_offsets = other.protocol_offsets;
//        other.protocol_offsets.clear();
//        curr_offset = other.curr_offset;
//        other.curr_offset = 0;
//    }

    received_msg& operator=(const received_msg& other) {
        int cnt = other.data.size();
        data = udata_t(cnt, 0);
        memcpy(data.data(), other.data.c_str(), cnt);

        protocol_offsets = other.protocol_offsets;
        curr_offset = other.curr_offset;

        return *this;
    }

//    received_msg& operator=(received_msg&& other) {
//        data = other.data;
//        other.data.clear();
//
//        protocol_offsets = other.protocol_offsets;
//        other.protocol_offsets.clear();
//        curr_offset = other.curr_offset;
//        other.curr_offset = 0;
//
//        return *this;
//    }

};

using msg_recv_listener = basic_recv_listener<received_msg>;

#endif //SERVERCLIENT_RECEIVED_MSG_H
