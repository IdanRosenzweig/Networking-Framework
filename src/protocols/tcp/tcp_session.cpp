#include "tcp_session.h"
#include "../../linux/error_codes.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

tcp_session_conn::tcp_session_conn(int _sd) : sd(_sd) {
    alive = true;

    worker = std::thread([&]() -> void {
        while (true) {
            std::this_thread::sleep_for(10ms);

#define BUFF_SZ 1024
            uint8_t buff[BUFF_SZ];
            memset(buff, 0, BUFF_SZ);

            int data_sz = recv(this->sd, buff, BUFF_SZ, 0);
            if (data_sz == RECV_ERROR) {
                alive = false;
//                std::cerr << "tcp seseion couldn't read" << endl;
                break;
            } else if (data_sz == 0) {
                alive = false;
//                std::cerr << "tcp session ended" << endl;
                break;
            }

//            std::cout << "tcp session read size " << data_sz << endl;

            recv_msg_t msg;
            msg.buff = udata_t(data_sz, 0x00);
            memcpy(msg.buff.data(), buff, data_sz);
            msg.curr_offset = 0;
//            uint8_t *alloc_msg = new uint8_t[data_sz];
//            memcpy(alloc_msg, buff, data_sz);
//
//            recv_msg_t plain_data;
//            plain_data.buff = unique_ptr<uint8_t>(alloc_msg);
//            plain_data.buff.size() = data_sz;
//            plain_data.curr_offset = 0;
            this->handle_callback(std::move(msg));
        }
    });
}

tcp_session_conn::~tcp_session_conn() {
    worker.detach();
    close(sd);
}

int tcp_session_conn::send_data(send_msg_t &&data) {
    if (!alive) return -1;

    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(sd, SOL_SOCKET, SO_ERROR, &error, &len) != 0) {
        std::cerr << "can't getsockopt" << std::endl;
        throw;
    }
    if (error != 0) {
        switch (error) {
            case EPIPE: { // connection was closed
                alive = false;
                break;
            }
            default: {
                std::cerr << "socket err" << std::endl;
                throw;
            }
        }
        return SEND_MEDIUM_ERROR;
    }

    int res = send(sd, data.get_active_buff(), data.get_count(), 0);
    if (res == SEND_ERROR || res == 0)
        return SEND_MEDIUM_ERROR;
    else
        return res;
}

void tcp_session_conn::handle_callback(recv_msg_t &&data) {
    recv_forwarder::handle_callback(std::move(data));
}




