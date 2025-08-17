#pragma once

#include "src/abstract/sending/send_medium.h"
#include "src/abstract/receiving/recv_listener.h"

#include <memory>
#include <cstdint>
using namespace std;


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
