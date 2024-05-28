#ifndef NETWORKING_VIRTUAL_IF_H
#define NETWORKING_VIRTUAL_IF_H

#include <thread>
#include "../../../abstract/gateway/gateway.h"
#include "../../../abstract/receiving/msg/received_msg.h"

class linux_virtual_iface : public msg_recv_listener {
    gateway* base_gw;
    int fd;

    std::thread worker;

    void handle_received_event(received_msg &&event) override;

public:
    explicit linux_virtual_iface(gateway *gw, string& iface_name);

    ~linux_virtual_iface();

};


#endif //NETWORKING_VIRTUAL_IF_H
