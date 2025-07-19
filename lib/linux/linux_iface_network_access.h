#pragma once

#include "src/abstract/network_access/network_access.h"
#include "linux_iface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <memory>
using namespace std;

struct linux_iface_network_access : public network_access<vector<uint8_t>, vector<uint8_t>> {
private:
    shared_ptr<linux_iface> iface;

public:
    linux_iface_network_access(shared_ptr<linux_iface> const& iface) : iface(iface) {
    }

    /* send access */
    shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
        return iface->get_send_access();
    }

    /* recv access */
    void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
        iface->set_recv_access(recv);
    }

};