#ifndef SERVERCLIENT_BASIC_CO_CLIENT_H
#define SERVERCLIENT_BASIC_CO_CLIENT_H

// a basic client
class basic_co_client {
    virtual void conn() = 0;
    virtual void disconn() = 0;
};

#endif //SERVERCLIENT_BASIC_CO_CLIENT_H
