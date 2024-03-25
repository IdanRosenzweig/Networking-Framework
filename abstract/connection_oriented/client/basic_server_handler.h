#ifndef SERVERCLIENT_BASIC_SERVER_HANDLER_H
#define SERVERCLIENT_BASIC_SERVER_HANDLER_H

#include "../../basic_conn.h"
#include <memory>

class basic_server_handler {
public:
    std::unique_ptr<basic_conn> net_host;
};

#endif //SERVERCLIENT_BASIC_SERVER_HANDLER_H
