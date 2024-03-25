#ifndef SERVERCLIENT_TCP_CONN_H
#define SERVERCLIENT_TCP_CONN_H

#include "../../basic_conn.h"

// used symmetrically for both client and server
class tcp_conn : public basic_conn {
    int port;
};

#endif //SERVERCLIENT_TCP_CONN_H
