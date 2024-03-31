#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H

#include <map>
#include <memory>

// a basic protocol that allow encapsulating sub protocols
template <typename PROT_T, typename PROT_DATA>
class basic_encapsulating_client {
//public:
//    virtual int send_encapsulated_data(void *buff, int count) = 0;
//
//    virtual int recv_encapsulated_data(void *buff, int count) = 0;


public:
    struct prot_data {
         PROT_DATA addit_data; // additional data
    };
    std::map<PROT_T, prot_data> prot_handlers;


    // receive the next msg of the encapsulated protocol.
    virtual int recv_prot_next_msg(PROT_T prot, void* buff, int count) = 0;

    // send message of protocol prot to client
    virtual int send_next_prot_msg(PROT_T prot, void* buff, int count) = 0;

};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H
