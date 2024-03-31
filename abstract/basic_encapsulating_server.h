#ifndef SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
#define SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H

#include <memory>
#include <map>

// a basic protocol that supports identifying sub protocols
// (ethernet has ethertype field, ip has protocol field)

template <typename ADDR_T, typename PORT_T, typename PORT_DATA>
class basic_encapsulating_server {
public:
    struct prot_data {
        ADDR_T last_client;
//        std::unique_ptr<char> data;

        PORT_DATA addit_data; // additional data
    };
    std::map<PORT_T, prot_data> prot_handlers;

    // receive the next msg of the encapsulated protocol.
    // put the data in port_handlers[prot].data,
    // and the client's data in port_handlers[prot].last_client
    virtual int recv_prot_next_msg(PORT_T prot, void* buff, int count) = 0;

    // send message of protocol prot to client
    virtual int send_next_prot_msg(PORT_T prot, ADDR_T client, void* buff, int count) = 0;
};

#endif //SERVERCLIENT_BASIC_ENCAPSULATING_SERVER_H
