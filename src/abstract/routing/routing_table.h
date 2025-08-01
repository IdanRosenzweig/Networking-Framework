#pragma once

#include "src/abstract/network_access/net_access_bytes.h"

#include <memory>
using namespace std;

template <typename NET_MASK_T, typename ADDR_T>
struct routing_table {
public:
    virtual ~routing_table() = default;

    virtual void add_routing_rule(NET_MASK_T const& net_mask, shared_ptr<net_access_bytes>&& net_access) = 0;

    virtual shared_ptr<net_access_bytes> find_route(ADDR_T const& addr) const = 0;
};