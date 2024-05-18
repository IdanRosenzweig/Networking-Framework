#include "tcp_session.h"
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>

using namespace std;

tcp_session::tcp_session(int _sd, tcp_session_data data) : sd(_sd), sessionData(data) {
    alive = true;

    worker = std::thread([&]() -> void {
        while (true) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10ms);

#define BUFF_SZ 1024
            uint8_t buff[BUFF_SZ];
            memset(buff, 0, BUFF_SZ);

            int data_sz = recv(this->sd, buff, BUFF_SZ, 0);
            if (data_sz == -1) {
                alive = false;
//                cerr << "tcp_client_server seseion couldn't read" << endl;
                break;
            } else if (data_sz == 0) {
                alive = false;
//                cerr << "tcp_client_server session ended" << endl;
                break;
            }

//            cout << "tcp_client_server session read size " << data_sz << endl;

            received_msg msg;
            msg.data = udata_t(data_sz, 0x00);
            memcpy(msg.data.data(), buff, data_sz);
            msg.curr_offset = 0;
//            uint8_t *alloc_msg = new uint8_t[data_sz];
//            memcpy(alloc_msg, buff, data_sz);
//
//            received_msg msg;
//            msg.data = unique_ptr<uint8_t>(alloc_msg);
//            msg.data.size() = data_sz;
//            msg.curr_offset = 0;
            this->handle_received_event(msg);
        }
    });
}

tcp_session::~tcp_session() {
//    cout << "tcp_session deconsturcotor" << endl;
    if (worker.joinable()) worker.detach();
    close(sd);
}

int tcp_session::send_data(send_msg<>& msg) {
    if (!alive) return -1;

    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(sd, SOL_SOCKET, SO_ERROR, &error, &len) != 0) {
        perror("getsockopt");
        throw;
    }
    if (error != 0) {
        switch (error) {
            case EPIPE: { // connection was closed
                alive = false;
                break;
            }
            default: throw "socket err";
        }
        return 0;
    }

    return send(sd, msg.get_active_buff(), msg.get_count(), 0);
}

void tcp_session::handle_received_event(received_msg &event) {
    multi_receiver::handle_received_event(event);
}




