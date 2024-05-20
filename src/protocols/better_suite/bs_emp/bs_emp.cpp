#include "bs_emp.h"
#include <iostream>
#include <netinet/in.h>
#include <cstring>
using namespace std;


int bs_emp::send_data(send_msg<>& msg) {
    uint8_t *buff = msg.get_reserve_buff();
    uint8_t *curr_buff = buff;

    curr_buff += encode_data(next_source_point.get_next_choice(), curr_buff);
    curr_buff += encode_data(next_dest_point.get_next_choice(), curr_buff);

    memcpy(curr_buff, msg.get_active_buff(), msg.get_count());
    curr_buff += msg.get_count();

    msg.toggle_active_buff();
    msg.set_count((int) (curr_buff - buff));
    return gateway->send_data(msg);
}

void bs_emp::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.data() + msg.curr_offset;
    uint8_t *curr_buff = buff;

    udata_t source;
    curr_buff += decode_data(&source, curr_buff);
    udata_t dest;
    curr_buff += decode_data(&dest, curr_buff);

    msg.protocol_offsets.push_back({msg.curr_offset, BS_PORT});
    msg.curr_offset += (int) (curr_buff - buff);

    if (default_handler != nullptr)
        default_handler->handle_received_event(msg);

    auto it = endpoints_handlers.search(dest);
    if (it != nullptr) it->key->handle_received_event(msg);

}