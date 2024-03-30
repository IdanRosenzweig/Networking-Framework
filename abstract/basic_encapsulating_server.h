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
        std::unique_ptr<char> data;

        PROT_DATA addit_data; // additional data
    };
    std::map<PROT_T, prot_data> prot_handlers;

    // register and setup a handler for receiving an encapsulated protocol.
    virtual void register_handler(PROT_T prot) = 0;


    // receive the next msg of the encapsulated protocol.
    // put the data in prot_handlers[prot].data,
    // and the client's data in prot_handlers[prot].last_client
    virtual void recv_prot_next_msg(PROT_T prot) = 0;

    // send message of protocol prot to client
    virtual void send_next_prot_msg(PROT_T prot, ADDR_T client, void* buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
