#ifndef SERVERCLIENT_MESSAGE_H
#define SERVERCLIENT_MESSAGE_H

#include <memory>
#include <stdint.h>

struct message {
    std::unique_ptr<uint8_t> data;
    int sz;
};

#endif //SERVERCLIENT_MESSAGE_H
