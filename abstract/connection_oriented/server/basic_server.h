#ifndef SERVERCLIENT_BASIC_SERVER_H
#define SERVERCLIENT_BASIC_SERVER_H


#include "basic_client_handler.h"
#include <type_traits>
#include <vector>

template <typename T> // T is a handler type for a connected client
class basic_server {
public:
    // connect next host
    virtual T conn_next_host() = 0;
};

#endif //SERVERCLIENT_BASIC_SERVER_H
