#ifndef SERVERCLIENT_PROTOCOL_QUEUE_H
#define SERVERCLIENT_PROTOCOL_QUEUE_H

#include "message_queue.h"
#include <map>

#define MAX_NO_MSG 1024

template <typename PROT_T>
struct protocol_queue {
    std::map<PROT_T, message_queue<MAX_NO_MSG>> prots;
};

#endif //SERVERCLIENT_PROTOCOL_QUEUE_H
