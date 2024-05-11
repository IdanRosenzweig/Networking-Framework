#ifndef SERVERCLIENT_VIRTUAL_IF_H
#define SERVERCLIENT_VIRTUAL_IF_H

#include <thread>
#include "../abstract/gateway/basic_gateway.h"
#include "../abstract/receiving/msg_receiver.h"

class linux_virtual_iface;

linux_virtual_iface create_virtual_iface_from_connection(basic_gateway *ggw, char dev[6]);

class linux_virtual_iface : public msg_receiver {
    basic_gateway* gateway;
    int fd;

    std::thread worker;

    explicit linux_virtual_iface(basic_gateway *ggw, char dev[6]);

    void handle_received_event(received_msg &event) override;

public:
    ~linux_virtual_iface();

    friend linux_virtual_iface create_virtual_iface_from_connection(basic_gateway *ggw, char dev[6]);

};


#endif //SERVERCLIENT_VIRTUAL_IF_H
