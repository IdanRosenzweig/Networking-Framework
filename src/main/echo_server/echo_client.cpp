#include "../../temp_prot_stacks/tcp_client_server/tcp_client.h"
#include "../../temp_prot_stacks/tcp_client_server/tcp_boundary_preserving_client.h"

#include <string>
#include <iostream>
using namespace std;

#define SERVER_ADDR "10.100.102.18"
#define SERVER_PORT 5678
#define MY_PORT 1212

class client_app_tcp : public msg_receiver {
public:
    void handle_received_event(received_msg &&event) override {
        cout << "server: " << event.data.data() + event.curr_offset << endl;
    }
};

void tcp_main() {

//    tcp_client client(SERVER_ADDR, SERVER_PORT, MY_PORT);
    tcp_boundary_preserving_client client(convert_to_ip4_addr(SERVER_ADDR), SERVER_PORT, MY_PORT);

    client_app_tcp app;
    client.add_listener(&app);

    std::cout << "sending msg" << std::endl;
#define BUFF_SZ 1024
    uint8_t buff[BUFF_SZ] = {0};

    sleep(1);
    {
        char* msg = "hello";
        int len = strlen(msg);
        memcpy(buff, msg, len);

        send_msg send;
        memcpy(send.get_active_buff(), msg, len);
        send.set_count(len);
        client.send_data(send);
    }

//    sleep(1);
    {
        char* msg = "this is second";
        int len = strlen(msg);
        memcpy(buff, msg, len);

        send_msg send;
        memcpy(send.get_active_buff(), msg, len);
        send.set_count(len);
        client.send_data(send);
    }

//    sleep(1);
    {
        char* msg = "third";
        int len = strlen(msg);
        memcpy(buff, msg, len);

        send_msg send;
        memcpy(send.get_active_buff(), msg, len);
        send.set_count(len);
        client.send_data(send);
    }

//    sleep(1);
    {
        char* msg = "fourth";
        int len = strlen(msg);
        memcpy(buff, msg, len);

        send_msg send;
        memcpy(send.get_active_buff(), msg, len);
        send.set_count(len);
        client.send_data(send);
    }

//    sleep(1);
    {
        char* msg = "fifth";
        int len = strlen(msg);
        memcpy(buff, msg, len);

        send_msg send;
        memcpy(send.get_active_buff(), msg, len);
        send.set_count(len);
        client.send_data(send);
    }

    sleep(1);

//    while (true) {
//
//    }

}

int main() {
    tcp_main();
}