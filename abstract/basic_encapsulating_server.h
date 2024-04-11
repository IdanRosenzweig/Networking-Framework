#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H

#include <memory>
#include <map>

// a basic protocol that supports identifying sub protocols
// (ethernet has ethertype field, ip has protocol field)

template <typename ADDR_T, typename PROT_T, typename PROT_DATA>
class basic_encapsulating_server {
public:
    struct prot_data {
        ADDR_T last_client;
//        std::unique_ptr<char> data;

        PROT_DATA addit_data; // additional data
    };
    std::map<PROT_T, prot_data> prot_handlers;


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


    // receive the next msg of the encapsulated protocol.
    // put the data in port_handlers[prot].data,
    // and the client's data in port_handlers[prot].last_client
    virtual int recv_next_msg(void* buff, int count) = 0;

    // send message of protocol prot to client
    virtual int send_next_msg(void* buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
