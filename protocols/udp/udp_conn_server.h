#ifndef SERVERCLIENT_UDP_CONN_SERVER_H
#define SERVERCLIENT_UDP_CONN_SERVER_H

#include <map>

#include "../../abstract/basic_send_conn.h"
#include "../../abstract/basic_recv_interrupt_handler.h"
#include "../../abstract/next_choice.h"

class udp_conn_server : public basic_send_conn, public basic_recv_interrupt_handler {
public:

    // send
    next_choice<int> server_port;
    next_choice<int> next_external_port;

    basic_send_conn* gateway;

    int send_data(void* buff, int cnt) override;

    // recv
    std::map<int, basic_recv_interrupt_handler*> port_handlers;

    void handle_received_msg(msg_protocol_stack &&msg) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
