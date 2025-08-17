#pragma once

#include "src/abstract/sending/send_medium.h"
#include "src/abstract/receiving/recv_listener.h"

#include <memory>
#include <vector>
#include <cstdint>
using namespace std;


/* send access, provided here, can be used outside to send to the access */
struct net_access_send {
public:
    virtual ~net_access_send() = default;

protected:
    virtual shared_ptr<send_medium_bytes> impl_get_send_access() = 0;

public:
    shared_ptr<send_medium_bytes> get_send_access() {
        return impl_get_send_access();
    }
};

shared_ptr<net_access_send> net_access_send_empty(); // make an empty send access (returns nullptr)
shared_ptr<net_access_send> net_access_send_make_static(shared_ptr<send_medium_bytes>&& static_send); // make a static net_access_send from send medium


/* recv access, can be set from outside in order to receive from the access */
struct net_access_recv {
protected:
    virtual void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) = 0;
// todo add has_recv_access, clear_recv_access, and throw is someones calls set_recv_access when there is already another access in place

public:
    void set_recv_access(shared_ptr<recv_listener_bytes> const& recv) {
        impl_set_recv_access(recv);
    }
};

shared_ptr<net_access_send> net_access_send_empty(); // make an empty recv access (setting the recv does nothing)


/* net access, composed of a send access and a recv access */
struct net_access : public net_access_send, public net_access_recv {
    virtual ~net_access() = default;
};

shared_ptr<net_access> net_access_make_empty(); // make an empty net access (same as empty send access and empty recv access)
shared_ptr<net_access> net_access_make_from_send_recv(shared_ptr<net_access_send>&& send, shared_ptr<net_access_recv>&& recv); // make a composed net access from a given send access and recv access
shared_ptr<net_access> net_access_override_send(shared_ptr<net_access>&& net_access, shared_ptr<net_access_send>&& send); // take a base net access, and overide its send access with a new one
shared_ptr<net_access> net_access_override_recv(shared_ptr<net_access>&& net_access, shared_ptr<net_access_recv>&& recv); // take a base net access, and overide its recv access with a new one
