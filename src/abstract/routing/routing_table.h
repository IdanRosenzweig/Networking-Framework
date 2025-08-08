#pragma once

#include "src/abstract/net_access/net_access.h"

#include <memory>
using namespace std;

template <typename NET_MASK_T, typename ADDR_T>
struct routing_table {
protected: // todo protected
    shared_ptr<net_access> default_gateway = nullptr;

    // find the appropriate route for an address (based only on the addded rules)
    virtual shared_ptr<net_access> impl_find_route(ADDR_T const& addr) const = 0; 

public:
    virtual ~routing_table() = default;

    // add a routing rule for the net mask
    virtual void add_routing_rule(NET_MASK_T const& net_mask, shared_ptr<net_access>&& net_access) = 0;

    // set the default route to use when nothing matches
    void set_default_gateway(shared_ptr<net_access>&& net_access) {
        default_gateway = std::move(net_access);
    }

    // find the appropriate route for an address (based on added rules and the default gateway)
    shared_ptr<net_access> find_route(ADDR_T const& addr) const {
        auto route = impl_find_route(addr);
        if (route != nullptr) return route;

        return default_gateway;
    }
    
};