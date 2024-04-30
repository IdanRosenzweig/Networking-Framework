#include "basic_forwarder_daemon.h"

int daemon_conn_side_handler::send_data(send_msg msg) {
    return daemon->conn_to_client->send_data(msg);
}

void daemon_conn_side_handler::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    if (cnt <= 0) return;

    cout << "sending raw bytes to data link layer" << endl;
    daemon->network_handler.send_data({buff, cnt});
}

int daemon_network_side_handler::send_data(send_msg msg) {
    return daemon->network_gateway->send_data(msg);
}

void daemon_network_side_handler::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    if (cnt <= 0) return;

    cout << "sending reply back to client" << endl;
    daemon->conn_handler.send_data({buff, cnt});
}