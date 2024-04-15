#ifndef SERVERCLIENT_MSG_PROTOCOL_STACK_H
#define SERVERCLIENT_MSG_PROTOCOL_STACK_H

#include "message.h"
#include <stack>
using namespace std;

struct msg_protocol_stack {
    message msg;
    stack<int> protocol_offsets; // previous offsets of encapsulating protocols
    int curr_offset; // offset to the current data
};

#endif //SERVERCLIENT_MSG_PROTOCOL_STACK_H
