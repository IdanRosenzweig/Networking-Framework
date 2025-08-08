#include "var_len_encoding.h"

int var_len_encode_bytes(uint8_t* dest_buff, vector<uint8_t> const& src_data) {
    uint8_t *curr_buff = dest_buff;
    int total_bytes = 0;

    uint8_t const* next_byte = src_data.data();
    int data_len = src_data.size();
    for (int data_cnt = 0; data_cnt < data_len; data_cnt++) {
        if (*next_byte == ENCODE_ESCAPE_BYTE) {
            *(curr_buff++) = ENCODE_ESCAPE_BYTE;
            *(curr_buff++) = *(next_byte++);
            total_bytes += 2;
        } else {
            *(curr_buff++) = *(next_byte++);
            total_bytes += 1;
        }
    }

    *(curr_buff++) = ENCODE_ESCAPE_BYTE;
    *(curr_buff++) = ENCODE_DELIMITER;
    total_bytes += 2;

    return total_bytes;
}

int var_len_decode_bytes(vector<uint8_t>* dest_data, uint8_t const* src_buff) {
    // using two-byte delimiter of ENCODE_ESCAPE_BYTE,ENCODE_DELIMITER
    // if ENCODE_ESCAPE_BYTE byte appears in the dest_data itself, it would be encoded as ENCODE_ESCAPE_BYTE,ENCODE_ESCAPE_BYTE

    dest_data->clear();
    int total_bytes = 0;

    const uint8_t* next_byte = (const uint8_t*) src_buff;
    while (true) {
        if (*next_byte == ENCODE_ESCAPE_BYTE) {
            next_byte++;
            total_bytes++;
            if (*next_byte == ENCODE_DELIMITER) {
                return ++total_bytes;
            } else {
                dest_data->push_back(*(next_byte++));
                total_bytes++;
            }
        } else {
            dest_data->push_back(*(next_byte++));
            total_bytes += 1;
        }
    }
}
