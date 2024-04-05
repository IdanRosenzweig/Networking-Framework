#ifndef SERVERCLIENT_PROTOCOL_QUEUE_H
#define SERVERCLIENT_PROTOCOL_QUEUE_H

#include "message_queue.h"
#include <map>

template <typename PROT_T>
struct protocol_queue {
    std::map<PROT_T, message_queue> prots;
};

#endif //SERVERCLIENT_PROTOCOL_QUEUE_H
