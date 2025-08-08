#pragma once

#include "src/abstract/sending/send_medium.h"
#include "src/abstract/receiving/recv_listener.h"

#include <memory>
#include <vector>
#include <cstdint>
using namespace std;

struct net_access {
    /* send access, provided by the iface access, can be used outside */
protected:
    virtual shared_ptr<send_medium_bytes> impl_get_send_access() = 0;

public:
    shared_ptr<send_medium_bytes> get_send_access() {
        return impl_get_send_access();
    }

    /* recv access, can be set from outside in order to receive from the iface */
protected:
    virtual void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) = 0;

public:
    void set_recv_access(shared_ptr<recv_listener_bytes> const& recv) {
        impl_set_recv_access(recv);
    }
};

shared_ptr<net_access> net_access_save_send_access(shared_ptr<net_access>&& net_access, shared_ptr<send_medium_bytes>&& send);
