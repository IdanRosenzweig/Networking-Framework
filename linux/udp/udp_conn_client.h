#ifndef SERVERCLIENT_UDP_CONN_CLIENT_H
#define SERVERCLIENT_UDP_CONN_CLIENT_H


#include "../ip4/ip4_conn_client.h"

class udp_conn_client {
protected:
    int my_port = 1212;
    int dest_port;

public:
    udp_conn_client(int dest_port, int my_port);

    ip4_conn_client* ip_client;

    int recv_data(void* data, int count);

    int send_data(void* buff, int cnt);

};

#endif //SERVERCLIENT_UDP_CONN_CLIENT_H
