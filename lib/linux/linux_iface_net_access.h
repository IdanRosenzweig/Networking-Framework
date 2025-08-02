#pragma once

#include "linux_iface.h"
#include "src/abstract/network_access/net_access_bytes.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
using namespace std;

struct linux_iface_net_access_bytes : public net_access_bytes {
private:
    shared_ptr<linux_iface> iface;

public:
    linux_iface_net_access_bytes(shared_ptr<linux_iface> const& iface) : iface(iface) {
    }

protected:
    /* send access */
    shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
        return iface->outgoing_get_send();
    }

    /* recv access */
    void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
        iface->incoming_set_recv(recv);
    }

};