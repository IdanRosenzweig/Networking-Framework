#ifndef SERVERCLIENT_PROTOCOL_QUEUE_H
#define SERVERCLIENT_PROTOCOL_QUEUE_H

#include "ring_buffer.h"
#include "message.h"
#include <map>

#define MAX_NO_MSG 1024

template <typename PROT_T>
struct protocol_queue {
    std::map<PROT_T, ring_buffer<message, MAX_NO_MSG>> prots;
};

#endif //SERVERCLIENT_PROTOCOL_QUEUE_H
