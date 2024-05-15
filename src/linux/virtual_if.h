#ifndef SERVERCLIENT_VIRTUAL_IF_H
#define SERVERCLIENT_VIRTUAL_IF_H

#include <thread>
#include "../abstract/gateway/msg_gateway.h"
#include "../abstract/receiving/msg/msg_receiver.h"

class linux_virtual_iface : private msg_receiver {
    msg_gateway* gateway;
    int fd;

    std::thread worker;

    void handle_received_event(received_msg &event) override;

public:
    explicit linux_virtual_iface(msg_gateway *gw, char dev[6]);

    ~linux_virtual_iface();

};


#endif //SERVERCLIENT_VIRTUAL_IF_H
