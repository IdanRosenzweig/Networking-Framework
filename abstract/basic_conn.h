#ifndef SERVERCLIENT_BASIC_CONN_H
#define SERVERCLIENT_BASIC_CONN_H

// a basic network connection
class basic_conn {
public:
    virtual int send_data(void* buff, int count) = 0;
    virtual int recv_data(void* buff, int count) = 0;

    virtual ~basic_conn() = default;
};

#endif //SERVERCLIENT_BASIC_CONN_H
