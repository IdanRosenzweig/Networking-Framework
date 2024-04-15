#ifndef SERVERCLIENT_BASIC_RECV_INTERRUPT_HANDLER_H
#define SERVERCLIENT_BASIC_RECV_INTERRUPT_HANDLER_H

#include "msg_protocol_stack.h"

// receiving data is different from sending data in the sense that
// you can control when to send data, but receiving data is rather interrupted.

// in addition, when sending data you don't haven to worry about the protocol stack (you control it),
// but when receiving data you have to process the protocol stack


// a basic network handler for handling interrupted data
class basic_recv_interrupt_handler {
public:
    // this function would be interrupted when receiving data
    virtual void handle_received_msg(msg_protocol_stack&& msg) = 0;
};

#endif //SERVERCLIENT_BASIC_RECV_INTERRUPT_HANDLER_H
