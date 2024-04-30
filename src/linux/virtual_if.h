#ifndef SERVERCLIENT_VIRTUAL_IF_H
#define SERVERCLIENT_VIRTUAL_IF_H

#include <thread>
#include "../abstract/connection/basic_gateway.h"
#include "../abstract/receiving/msg_receiver.h"


int linux_create_tap(char *dev);

class linux_virtual_iface : public msg_receiver {
    basic_gateway* gateway;
    int fd;

    std::thread worker;
public:
    explicit linux_virtual_iface(basic_gateway *gateway, char dev[6]);

    ~linux_virtual_iface();

    void handle_received_event(received_msg &event) override;
};


#endif //SERVERCLIENT_VIRTUAL_IF_H
