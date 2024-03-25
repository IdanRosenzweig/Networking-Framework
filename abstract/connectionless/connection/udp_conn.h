#ifndef SERVERCLIENT_UDP_CONN_H
#define SERVERCLIENT_UDP_CONN_H

#include "../../basic_conn.h"

// used symmetrically for both client and server
class udp_conn : public basic_conn {
    int port;
};

#endif //SERVERCLIENT_UDP_CONN_H
