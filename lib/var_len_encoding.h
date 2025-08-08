#pragma once

#include <vector>
#include <cstdint>
using namespace std;

#define ENCODE_ESCAPE_BYTE uint8_t(0x00)
#define ENCODE_DELIMITER uint8_t(0x01)

int var_len_encode_bytes(uint8_t* dest_buff, vector<uint8_t> const& src_data);

int var_len_decode_bytes(vector<uint8_t>* dest_data, uint8_t const* src_buff);

template <typename NUM_TYPE>
int var_len_encode_num(uint8_t* buff, NUM_TYPE num) {
    uint8_t * curr_buff = (uint8_t*) buff;
    if (num == 0) {
        *curr_buff = 0;
        return 1;
    }

    int cnt = 0;
    while (num != 0) {
        uint8_t next_byte = 0;
        next_byte |= num & (~((uint8_t) 0) ^ (((uint8_t) 1) << 7));
        num >>= 7;

        if (num != 0) next_byte |= ((uint8_t) 1) << 7;

        *(curr_buff++) = next_byte;
        cnt++;
    }

    return cnt;
}

template <typename NUM_TYPE>
int var_len_decode_num(NUM_TYPE* dest_num, uint8_t const* buff) {
    uint8_t const* curr_buff = buff;
    if (*curr_buff == 0) {
        *dest_num = 0;
        return 1;
    }

    NUM_TYPE res_num = 0;

    int res_cnt = 0;
    int bits_cnt = 0;
    while (true) {
        uint8_t next_bits = *(curr_buff) & (~((uint8_t) 0) ^ (((uint8_t) 1) << 7));

        res_num |= next_bits << bits_cnt;
        res_cnt++;
        bits_cnt += 7;

        bool has_next = *(curr_buff++) & (((uint8_t) 1) << 7);
        if (!has_next) break;
    }

    *dest_num = res_num;
    return res_cnt;
}