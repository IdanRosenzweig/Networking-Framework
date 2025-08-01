#pragma once

#include <memory>
using namespace std;

#include "src/abstract/network_access/net_access_bytes.h"

struct data_link_layer_net_access : public net_access_bytes {
private:
    shared_ptr<net_access_bytes> net_access;

public:
    data_link_layer_net_access(shared_ptr<net_access_bytes>&& net_access) : net_access(std::move(net_access)) {
    }

protected:
    shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
        return net_access->get_send_access();
    }

    void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
        net_access->set_recv_access(recv);
    }
};