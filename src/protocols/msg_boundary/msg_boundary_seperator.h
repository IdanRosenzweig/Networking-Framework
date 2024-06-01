#ifndef NETWORKING_MSG_BOUNDARY_SEPERATOR_H
#define NETWORKING_MSG_BOUNDARY_SEPERATOR_H

#include "../../abstract/connection/connection.h"

// takes a connection that doesn't necessarily preserve message boundary
// and wraps it with a message boundary preserving protocol.

// it also deletes all subprotocols that are stored in the messages coming from the connection (in plain_data.protocol_offsets)

// THIS CLASS ASSUMES EACH PACKET WOULD CONTAIN AT LEAST SIZEOF(MSG_SZ_FIELD) BYTES

template<typename MSG_SZ_FIELD = uint16_t>
class msg_boundary_seperator : public connection {
    connection *base_conn;
public:

    msg_boundary_seperator() {}

    explicit msg_boundary_seperator(connection *baseConn) : base_conn(baseConn) {
        base_conn->add_listener(this);
    }

//    msg_boundary_seperator(msg_boundary_seperator&& other) : base_conn(other.base_conn) {
//        base_conn->add_listener(this);
//        mid_packet = other.mid_packet;
//        curr_msg = other.curr_msg;
//        curr_read = other.curr_read;
//    }

    ~msg_boundary_seperator() {
        base_conn->remove_listener(this);
    }

    int send_data(send_msg_t &&data) override {
        uint8_t *buff = data.get_reserve_buff();

        MSG_SZ_FIELD sz = data.get_count();
        *(MSG_SZ_FIELD *) buff = sz;
        memcpy(buff + sizeof(MSG_SZ_FIELD), data.get_active_buff(), sz);

        size_t total_sz = sz + sizeof(MSG_SZ_FIELD);
        data.toggle_active_buff();
        data.set_count(total_sz);
        return base_conn->send_data(std::move(data));
    }

    bool mid_packet = false; // in the middle of reading a packet
    recv_msg_t curr_msg; // curr constructing packet. contains the size of the curr packet and the allocated buffer for its data_t
    MSG_SZ_FIELD curr_read; // amount of bytes received so far for the curr packet

    void handle_callback(recv_msg_t &&data) override {
        uint8_t *buff = data.buff_at_curr_offset();
        MSG_SZ_FIELD cnt = data.buff_cnt_at_curr_offset();

        while (cnt > 0) {
            if (!mid_packet) {
                MSG_SZ_FIELD curr_sz = *(MSG_SZ_FIELD *) buff;
                buff += sizeof(MSG_SZ_FIELD);
                cnt -= sizeof(MSG_SZ_FIELD);

                curr_msg = recv_msg_t();
                curr_msg.buff = udata_t(curr_sz, 0x00);
                curr_msg.curr_offset = 0;
                curr_msg.protocol_offsets.clear();

                curr_read = 0;
                mid_packet = true;
            }

            MSG_SZ_FIELD read_left = curr_msg.buff.size() - curr_read;
            MSG_SZ_FIELD reading = min(cnt, read_left);

            memcpy(curr_msg.buff.data() + curr_read, buff, reading);
            curr_read += reading;
            buff += reading;
            cnt -= reading;

            if (curr_read == curr_msg.buff.size()) {
                recv_forwarder::handle_callback(std::move(curr_msg));
                mid_packet = false;
            }
        }
    }
};


#endif //NETWORKING_MSG_BOUNDARY_SEPERATOR_H
