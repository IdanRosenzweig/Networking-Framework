#ifndef SERVERCLIENT_PROTOCOL_MULTIPLEXER_H
#define SERVERCLIENT_PROTOCOL_MULTIPLEXER_H

#include "ring_buffer.h"
#include "message.h"
#include <map>

#define MAX_NO_MSG 1024

template <typename PROT_T, typename PROT_DATA>
struct protocol_multiplexer {
    std::map<PROT_T, PROT_DATA> prots;
};

#endif //SERVERCLIENT_PROTOCOL_MULTIPLEXER_H
