#include "tcp_session.h"
#include <sys/types.h>
#include <sys/socket.h>

#include <iostream>
using namespace std;

tcp_session::tcp_session(int sd, ip4_addr destAddr, int sourcePort, int destPort) : sd(sd), dest_addr(destAddr),
                                                                                           source_port(sourcePort),
                                                                                           dest_port(destPort) {

    worker = std::thread([&]() -> void {
        while (true) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(300ms);

#define BUFF_SZ 1024
            char buff[BUFF_SZ];
            memset(buff, 0, BUFF_SZ);

            int data_sz = recv(this->sd, buff, BUFF_SZ, 0);;
            if (data_sz == 0) {
                cerr << "tcp session ended" << endl;
                break;
            } else if (data_sz == -1) {
                cerr << "tcp seseion couldn't read" << endl;
                continue;
            }

            uint8_t *alloc_msg = new uint8_t[data_sz];
            memcpy(alloc_msg, buff, data_sz);

            received_msg msg;
            msg.data = unique_ptr<uint8_t>(alloc_msg);
            msg.sz = data_sz;
            msg.curr_offset = 0;
            this->handle_received_event(msg);
        }
    });
}

tcp_session::~tcp_session() {
    worker.detach();
    close(sd);
}

int tcp_session::send_data(send_msg msg) {
    return send(sd, msg.buff, msg.count, 0);
}

void tcp_session::handle_received_event(received_msg& event) {
    listenable::handle_received_event(event);
}




