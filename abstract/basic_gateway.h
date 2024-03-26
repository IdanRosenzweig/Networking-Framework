#ifndef SERVERCLIENT_BASIC_GATEWAY_H
#define SERVERCLIENT_BASIC_GATEWAY_H

#include "basic_conn.h"

class basic_gateway : public basic_conn {
public:
    int send_data(void *buff, int count) override {
        return send_raw(buff, count);
    }

    int recv_data(void *buff, int count) override {
        return recv_raw(buff, count);
    }

    virtual int send_raw(void *buff, int count) = 0;

    virtual int recv_raw(void *buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_GATEWAY_H
