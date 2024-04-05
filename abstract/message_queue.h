#ifndef SERVERCLIENT_MESSAGE_QUEUE_H
#define SERVERCLIENT_MESSAGE_QUEUE_H

#include "message.h"
#include <queue>

struct message_queue {
    std::queue<message> queue;
};

#endif //SERVERCLIENT_MESSAGE_QUEUE_H
