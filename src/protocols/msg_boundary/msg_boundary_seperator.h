#ifndef SERVERCLIENT_MSG_BOUNDARY_SEPERATOR_H
#define SERVERCLIENT_MSG_BOUNDARY_SEPERATOR_H

#include "../../abstract/connection/connection.h"

// takes a connection that doesn't necessarily preserve message boundary
// and wraps it with a message boundary preserving protocol.

// it also deletes all subprotocols that are stored in the messages coming from the connection (in msg.protocol_offsets)

template <typename MSG_SZ_FIELD = uint16_t>
class msg_boundary_seperator : public connection {
    connection* base_conn;
public:

    msg_boundary_seperator() {}
    explicit msg_boundary_seperator(connection *baseConn) : base_conn(baseConn) {
        base_conn->add_listener(this);
    }

    int send_data(send_msg<>&& val) override {
        MSG_SZ_FIELD sz = val.get_count();

        uint8_t *buff = val.get_reserve_buff();

        *(MSG_SZ_FIELD*) buff = sz;
        memcpy(buff + sizeof(MSG_SZ_FIELD), val.get_active_buff(), sz);

        int total_sz = sz + sizeof(MSG_SZ_FIELD);
        val.toggle_active_buff();
        val.set_count(total_sz);
        return base_conn->send_data(std::move(val));
    }

    bool mid_packet = false; // in the middle of reading a packet
    received_msg curr_msg; // curr constructing packet. contains the size of the curr packet and the allocated buffer for its data_t
    MSG_SZ_FIELD curr_read; // amount of bytes received so far for the curr packet

    void handle_received_event(received_msg &&event) override {
        uint8_t *buff = event.data.data() + event.curr_offset;
        MSG_SZ_FIELD cnt = event.data.size() - event.curr_offset;

        while (cnt > 0) {
            if (!mid_packet) {
                MSG_SZ_FIELD curr_sz = *(MSG_SZ_FIELD*)buff; // todo fix assuming each send would contain at least sizeof(uint16_t) bytes
                buff += sizeof(MSG_SZ_FIELD);
                cnt -= sizeof(MSG_SZ_FIELD);

                curr_msg = received_msg(); // todo fix?
                curr_msg.data = udata_t(curr_sz, 0x00);
                curr_msg.curr_offset = 0;
                curr_msg.protocol_offsets.clear();

                curr_read = 0;
                mid_packet = true;
            }

            MSG_SZ_FIELD read_left = curr_msg.data.size() - curr_read;
            MSG_SZ_FIELD reading = min(cnt, read_left);

            memcpy(curr_msg.data.data() + curr_read, buff, reading);
            curr_read += reading;
            buff += reading;
            cnt -= reading;

            if (curr_read == curr_msg.data.size()) {
                receive_multiplexer::handle_received_event(std::move(curr_msg));
                mid_packet = false;
            }
        }
    }
};


#endif //SERVERCLIENT_MSG_BOUNDARY_SEPERATOR_H
