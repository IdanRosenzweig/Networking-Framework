#pragma once

#include "linux_iface.h"
#include "src/abstract/net_access/net_access.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
using namespace std;

struct linux_iface_net_access : public net_access {
private:
    shared_ptr<linux_iface> iface;

public:
    linux_iface_net_access(shared_ptr<linux_iface> const& iface) : iface(iface) {
    }

protected:
    /* send access */
    shared_ptr<send_medium_bytes> impl_get_send_access() override {
        return iface->outgoing_get_send();
    }

    /* recv access */
    void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
        iface->incoming_set_recv(recv);
    }

};