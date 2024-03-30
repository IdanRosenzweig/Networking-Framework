#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H

// a basic protocol that allow encapsulating sub protocols
class basic_encapsulating_client {
public:
    virtual int send_encapsulated_data(void *buff, int count) = 0;

    virtual int recv_encapsulated_data(void *buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H
