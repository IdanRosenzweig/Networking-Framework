#include "../../temp_connections/tcp_client_server/tcp_client.h"
#include "../../temp_connections/tcp_client_server/tcp_boundary_preserving_client.h"

#include <string>
#include <iostream>
using namespace std;

#define SERVER_ADDR "10.100.102.18"
#define SERVER_PORT 5678
#define MY_PORT 1212

class client_app_tcp : public msg_receiver {
public:
    void handle_received_event(received_msg &event) override {
        cout << "raw_tcp_server: " << event.data.data() + event.curr_offset << endl;
    }
};

void tcp_main() {

//    tcp_client client(SERVER_ADDR, SERVER_PORT, MY_PORT);
    tcp_boundary_preserving_client client(convert_to_ip4_addr(SERVER_ADDR), SERVER_PORT, MY_PORT);

    client_app_tcp app;
    client.add_listener(&app);

    std::cout << "sending msg" << std::endl;
    char* msg;

    sleep(1);
    msg = "hello";
    send_msg send{msg, (int) strlen(msg)};
    client.send_data(send);

    sleep(1);
    msg = "thisis test msg";
    send = send_msg{msg, (int) strlen(msg)};
    client.send_data(send);

    sleep(1);
    msg = "another one";
    send = send_msg{msg, (int) strlen(msg)};
    client.send_data(send);

    sleep(1);
    msg = "another one2";
    send = send_msg{msg, (int) strlen(msg)};
    client.send_data(send);

    sleep(1);
    msg = "another one3";
    send = send_msg{msg, (int) strlen(msg)};
    client.send_data(send);

//    while (true) {
//
//    }

}

int main() {
    tcp_main();
}