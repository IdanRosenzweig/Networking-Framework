#ifndef SERVERCLIENT_ETHERNET_CONN_SERVER_H
#define SERVERCLIENT_ETHERNET_CONN_SERVER_H

#include <map>

#include "mac_addr.h"
#include "../../abstract/next_choice.h"
#include "../../abstract/basic_send_conn.h"
#include "../../abstract/basic_recv_interrupt_handler.h"

class ethernet_conn_server : public basic_send_conn, public basic_recv_interrupt_handler {
public:
    ethernet_conn_server();

    // send
    next_choice<int> next_protocol;
    next_choice<mac_addr> next_dest_addr;
    next_choice<mac_addr> my_addr;

    basic_send_conn* gateway;

    int send_data( void* buff, int count) override;

    // recv
    std::map<int, basic_recv_interrupt_handler*> prot_handlers;

    void handle_received_msg(msg_protocol_stack &&msg) override;

};


#endif //SERVERCLIENT_ETHERNET_CONN_SERVER_H
