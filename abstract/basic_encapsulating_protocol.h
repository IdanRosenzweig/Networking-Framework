#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_PROTOCOL_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_PROTOCOL_H

//#include "basic_conn.h"

class basic_encapsulating_protocol {
public:
//    int send_data(void *buff, int count) override {
//        return send_encapsulated_data(buff, count);
//    }
//
//    int recv_data(void *buff, int count) override {
//        return recv_encapsulated_data(buff, count);
//    }

    virtual int send_encapsulated_data(void *buff, int count) = 0;

    virtual int recv_encapsulated_data(void *buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_PROTOCOL_H
