#ifndef NETWORKING_BASIC_SNIFF_HANDLER_H
#define NETWORKING_BASIC_SNIFF_HANDLER_H

#include "../receiving/basic_recv_listener.h"
#include "../receiving/msg/received_msg.h"

class basic_sniff_handler : public basic_recv_listener<received_msg> {
};

#endif //NETWORKING_BASIC_SNIFF_HANDLER_H
