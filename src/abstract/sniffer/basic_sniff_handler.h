#ifndef NETWORKING_BASIC_SNIFF_HANDLER_H
#define NETWORKING_BASIC_SNIFF_HANDLER_H

#include "../receiving/basic_recv_listener.h"
#include "../receiving/msg/recv_msg_t.h"

class basic_sniff_handler : public basic_recv_listener<recv_msg_t> {
};

#endif //NETWORKING_BASIC_SNIFF_HANDLER_H
