#ifndef SERVERCLIENT_BASIC_CLIENT_HANDLER_H
#define SERVERCLIENT_BASIC_CLIENT_HANDLER_H

#include "../../basic_conn.h"
#include <memory>

class basic_client_handler {
public:
    std::unique_ptr<basic_conn> net_host;
    int id;
};

#endif //SERVERCLIENT_BASIC_CLIENT_HANDLER_H
