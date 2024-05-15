#ifndef SERVERCLIENT_BASIC_GATEWAY_H
#define SERVERCLIENT_BASIC_GATEWAY_H

#include "../connection/basic_connection.h"

template <typename SEND_T, typename RECV_T>
using basic_gateway = basic_connection<SEND_T, RECV_T>;

#endif //SERVERCLIENT_BASIC_GATEWAY_H
