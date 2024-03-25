#ifndef SERVERCLIENT_IP4_CONN_H
#define SERVERCLIENT_IP4_CONN_H

#include "../../basic_conn.h"
#include <string>

// used symmetrically for both client and server
class ip4_conn : public basic_conn {
    std::string addr;
};

#endif //SERVERCLIENT_IP4_CONN_H
