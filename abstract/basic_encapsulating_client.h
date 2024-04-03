#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H

#include <map>
#include <memory>

// a basic protocol that allow encapsulating sub protocols
template<typename PROT_T, typename PROT_DATA>
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


private:
    PROT_T next_prot; // indicated the protocol used in the next encapsulated message
public:
    PROT_T getNextProt() const {return next_prot;}

    void setNextProt(PROT_T nextProt) {next_prot = nextProt;}


    // receive the next msg (depending on the current encapsulated protocol).
    virtual int recv_next_msg(void *buff, int count) = 0;

    // send message (depending on the current encapsulated protocol).
    virtual int send_next_msg(void *buff, int count) = 0;

};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_CLIENT_H
