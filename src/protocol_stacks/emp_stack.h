#ifndef NETWORKING_EMP_STACK_H
#define NETWORKING_EMP_STACK_H

#include "../protocols/ip4/ip4_protocol.h"
#include "../protocols/emp/emp_protocol.h"

struct emp_protocol_stack {
    ip4_protocol ip_prot;
    emp_protocol emp_prot;
};

struct emp_packet_stack {
    ip4_addr source_addr;
    ip4_addr dest_addr;
    udata_t source_point;
    udata_t dest_point;
    recv_msg_t plain_data;
};

#endif //NETWORKING_EMP_STACK_H
