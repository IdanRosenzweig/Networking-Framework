#ifndef SERVERCLIENT_ETHERNET_CONN_CLIENT_H
#define SERVERCLIENT_ETHERNET_CONN_CLIENT_H

#include <map>

#include "mac_addr.h"

#include "../../abstract/basic_send_conn.h"
#include "../../abstract/message.h"
#include "../../abstract/next_choice.h"
#include "../../abstract/basic_recv_interrupt_handler.h"

class ethernet_conn_client : public basic_send_conn, public basic_recv_interrupt_handler {
public:

    // send
    next_choice<int> next_protocol;
    next_choice<mac_addr> next_mac_addr;
    next_choice<mac_addr> my_addr;

    basic_send_conn* gateway = nullptr;

    int send_data( void* buff, int count) override;

    // recv
    std::map<int, basic_recv_interrupt_handler*> prot_handlers;

    void handle_received_msg(msg_protocol_stack &&msg) override;

};
#endif //SERVERCLIENT_ETHERNET_CONN_CLIENT_H
