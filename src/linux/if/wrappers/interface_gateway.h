#ifndef NETWORKING_INTERFACE_GATEWAY_H
#define NETWORKING_INTERFACE_GATEWAY_H

#include <netpacket/packet.h>

#include "../../../abstract/gateway/gateway.h"
#include "../iface_access_point.h"

// wrapper class for the raw interface access point, providing gateway api
class interface_gateway : public gateway {
    weak_ptr<iface_access_point> iface_access;

public:
    interface_gateway(const weak_ptr<iface_access_point>& access);

    ~interface_gateway();

    int send_data(send_msg<>&& msg) override;

};

#endif //NETWORKING_INTERFACE_GATEWAY_H
