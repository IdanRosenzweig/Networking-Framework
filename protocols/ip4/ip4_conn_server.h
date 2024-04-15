#ifndef SERVERCLIENT_IP4_CONN_SERVER_H
#define SERVERCLIENT_IP4_CONN_SERVER_H

#include <map>

#include "ip4_addr.h"
#include "../../abstract/basic_send_conn.h"
#include "../../abstract/basic_recv_interrupt_handler.h"
#include "../../abstract/next_choice.h"

class ip4_conn_server : public basic_send_conn, public basic_recv_interrupt_handler {
public:

    // send
    next_choice<int> next_external_protocol;
    next_choice<ip4_addr> next_client_addr;

    basic_send_conn* gateway;

    int send_data( void* buff, int count) override;

    // recv
    std::map<int, basic_recv_interrupt_handler*> prot_handlers;

    void handle_received_msg(msg_protocol_stack &&msg) override;

};

#endif //SERVERCLIENT_IP4_CONN_SERVER_H
