#ifndef SERVERCLIENT_BASIC_SEND_CONN_H
#define SERVERCLIENT_BASIC_SEND_CONN_H

// a basic network gateway to send data
class basic_send_conn {
public:
    virtual int send_data(void* buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_SEND_CONN_H
