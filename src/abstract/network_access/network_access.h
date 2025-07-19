#pragma once

#include "src/abstract/sending/basic_send_medium.h"
#include "src/abstract/receiving/basic_recv_listener.h"

#include <memory>
#include <vector>
#include <cstdint>
using namespace std;

template <typename SEND_T, typename RECV_T>
struct network_access {
    /* send access, provided by the iface access, can be used outside */
protected:
    // shared_ptr<basic_send_medium<vector<uint8_t>>> send_access;
    virtual shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() = 0;

public:
    shared_ptr<basic_send_medium<vector<uint8_t>>> get_send_access() {
        return impl_get_send_access();
    }

    /* recv access, can be set from outside in order to receive from the iface */
protected:
    // shared_ptr<basic_recv_listener<vector<uint8_t>>> recv_access;
    virtual void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) = 0;

public:
    void set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) {
        impl_set_recv_access(recv);
    }
};

// network access for sending raw bytes
using network_access_bytes = network_access<vector<uint8_t>, vector<uint8_t>>;