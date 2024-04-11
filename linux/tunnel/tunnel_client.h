#ifndef SERVERCLIENT_TUNNEL_CLIENT_H
#define SERVERCLIENT_TUNNEL_CLIENT_H

#include "../udp/udp_conn_client.h"
#include "../tcp/tcp_conn_client.h"

class tunnel_client : public basic_conn {
public:

    udp_conn_client* client;

    int recv_data(void* data, int count) override;

    int send_data(void* buff, int cnt) override;
};

#endif //SERVERCLIENT_TUNNEL_CLIENT_H
