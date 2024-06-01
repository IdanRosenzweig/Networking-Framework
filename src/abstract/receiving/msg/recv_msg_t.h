#ifndef NETWORKING_RECV_MSG_T_H
#define NETWORKING_RECV_MSG_T_H

#include "../../utils/udata.h"
#include "../basic_recv_listener.h"
#include "../recv_forwarder.h"

#include <vector>
#include <cstring>
using namespace std;

using recv_msg_protocol_t = size_t;

struct recv_msg_t {
    udata_t buff;

    std::vector<std::pair<size_t, recv_msg_protocol_t>> protocol_offsets; // previous offsets of encapsulating protocols
    size_t curr_offset; // offset to the current plain_data

    recv_msg_t() : buff(), protocol_offsets(), curr_offset(0) {}

    recv_msg_t(uint8_t* ptr, size_t cnt) : buff(ptr, cnt), protocol_offsets(), curr_offset(0) {}

    recv_msg_t(const recv_msg_t& other) {
        size_t cnt = other.buff.size();
        buff = udata_t(cnt, 0);
        memcpy(buff.data(), other.buff.c_str(), cnt);

        protocol_offsets = other.protocol_offsets;
        curr_offset = other.curr_offset;
    }

//    recv_msg_t(recv_msg_t&& other) {
//        buff = other.buff;
//        other.buff.clear();
//
//        protocol_offsets = other.protocol_offsets;
//        other.protocol_offsets.clear();
//        curr_offset = other.curr_offset;
//        other.curr_offset = 0;
//    }

    recv_msg_t& operator=(const recv_msg_t& other) {
        int cnt = other.buff.size();
        buff = udata_t(cnt, 0);
        memcpy(buff.data(), other.buff.c_str(), cnt);

        protocol_offsets = other.protocol_offsets;
        curr_offset = other.curr_offset;

        return *this;
    }

//    recv_msg_t& operator=(recv_msg_t&& other) {
//        buff = other.buff;
//        other.buff.clear();
//
//        protocol_offsets = other.protocol_offsets;
//        other.protocol_offsets.clear();
//        curr_offset = other.curr_offset;
//        other.curr_offset = 0;
//
//        return *this;
//    }

    // ptr to the buffer corresponding to the current offset
    inline uint8_t* buff_at_curr_offset() {
        return buff.data() + curr_offset;
    }

    // number of bytes left at the current offset
    inline size_t buff_cnt_at_curr_offset() {
        return buff.size() - curr_offset;
    }

    // ptr to the buffer corresponding to the prot_in-th protocol
    inline uint8_t* buff_of_prot(size_t prot_in) {
        return buff.data() + protocol_offsets[prot_in].first;
    }

    // number of bytes left at the prot_in-th offset
    inline size_t buff_cnt_of_prot(size_t prot_in) {
        return buff.size() - protocol_offsets[prot_in].first;
    }

    // ptr to the buffer corresponding to the prot_in-th protocol (from the end)
    inline uint8_t* buff_of_prot_from_end(size_t prot_in) {
        return buff.data() + protocol_offsets[protocol_offsets.size() - 1 - prot_in].first;
    }

    // number of bytes left at the prot_in-th offset (from the end)
    inline size_t buff_cnt_of_prot_from_end(size_t prot_in) {
        return buff.size() - protocol_offsets[protocol_offsets.size() - 1 - prot_in].first;
    }

};

using msg_recv_listener = basic_recv_listener<recv_msg_t>;
using msg_recv_forwarder = recv_forwarder<recv_msg_t>;

#endif //NETWORKING_RECV_MSG_T_H
