#ifndef SERVERCLIENT_IP4_CONN_CLIENT_H
#define SERVERCLIENT_IP4_CONN_CLIENT_H

#include <map>

#include "ip4_addr.h"

#include "../../abstract/next_choice.h"
#include "../../abstract/basic_send_conn.h"
#include "../../abstract/basic_recv_interrupt_handler.h"

class ip4_conn_client : public basic_send_conn, public basic_recv_interrupt_handler {
public:

    // send
    next_choice<int> next_protocol;
    next_choice<ip4_addr> next_server_addr;

    basic_send_conn * gateway = nullptr;

    int send_data(void *buff, int count) override;

    // recv
    std::map<int, basic_recv_interrupt_handler*> prot_handlers;
    // todo it is also possible to assign handlers for specific addreses instead of protocols

    void handle_received_msg(msg_protocol_stack &&msg) override;

};

#endif //SERVERCLIENT_IP4_CONN_CLIENT_H
