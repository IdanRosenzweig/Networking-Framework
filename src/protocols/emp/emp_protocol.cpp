#include "emp_protocol.h"

#include "../protocol_t.h"

#include <iostream>
#include <cstring>
using namespace std;

int emp_protocol::send_data(send_msg_t &&data) {
    uint8_t *buff = data.get_reserve_buff();
    uint8_t *curr_buff = buff;

    curr_buff += encode_data(next_source_point.get_next_choice(), curr_buff);
    curr_buff += encode_data(next_dest_point.get_next_choice(), curr_buff);

    memcpy(curr_buff, data.get_active_buff(), data.get_count());
    curr_buff += data.get_count();

    data.toggle_active_buff();
    data.set_count((int) (curr_buff - buff));
    return send_medium.send_data(std::move(data));
}

void emp_protocol::handle_callback(recv_msg_t &&data) {
    uint8_t *buff = data.buff_at_curr_offset();
    uint8_t *curr_buff = buff;

    udata_t source;
    curr_buff += decode_data(&source, curr_buff);
    udata_t dest;
    curr_buff += decode_data(&dest, curr_buff);

    data.protocol_offsets.emplace_back(data.curr_offset, EMP);
    data.curr_offset += (size_t) (curr_buff - buff);


    if (default_handler != nullptr) {
        recv_msg_t copy(data);
        default_handler->handle_callback(std::move(copy));
    }

    auto it = endpoints_handlers.search(dest);
    if (it != nullptr)
        for (auto &handler: it->key) {
            recv_msg_t copy(data);
            handler->handle_callback(std::move(copy));
        }

}