#pragma once

#include <memory>
using namespace std;

#include "src/abstract/net_access/net_access.h"

struct data_link_layer_net_access : public net_access {
private:
    shared_ptr<net_access> net_access;

public:
    data_link_layer_net_access(shared_ptr<net_access>&& net_access) : net_access(std::move(net_access)) {
    }

protected:
    shared_ptr<send_medium_bytes> impl_get_send_access() override {
        return net_access->get_send_access();
    }

    void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
        net_access->set_recv_access(recv);
    }
};