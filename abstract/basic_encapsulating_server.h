#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H

#include <memory>
#include <map>

// a basic protocol that supports identifying sub protocols
// (ethernet has ethertype field, ip has protocol field)

template <typename PROT_T, typename ADDR_T>
class basic_encapsulating_server {
private:
    PROT_T next_prot; // indicated the protocol used in the next encapsulated message
public:
    PROT_T getNextProt() const {return next_prot;}

    void setNextProt(PROT_T nextProt) {next_prot = nextProt;}

private:
    ADDR_T next_client = {0}; // indicates the client to send the next packet to
public:
    ADDR_T getNextClient() const {return next_client;}

    void setNextClient(ADDR_T nextClient) {next_client = nextClient;}


    // receive the next msg (depending on the marked encapsulated protocol and client).
    virtual int recv_next_msg(void* buff, int count) = 0;

    // send message of protocol prot to next_client
    virtual int send_next_msg(void* buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
