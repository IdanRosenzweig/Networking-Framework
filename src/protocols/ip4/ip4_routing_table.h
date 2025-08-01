#pragma once

#include "ip4_addr.h"

#include "src/abstract/routing/routing_table.h"

struct ip4_routing_table : public routing_table<ip4_subnet_mask, ip4_addr> {
private:
    vector<pair<ip4_subnet_mask, shared_ptr<net_access_bytes>>> rules;

public:
    void add_routing_rule(ip4_subnet_mask const& subnet, shared_ptr<net_access_bytes>&& net_access) override {
        rules.push_back({subnet, std::move(net_access)});
    }

    shared_ptr<net_access_bytes> find_route(ip4_addr const& addr) const override {
        optional<ip4_subnet_mask> closest_subnet = {};
        shared_ptr<net_access_bytes> closest_route = nullptr;

        for (auto& [subnet, route] : rules) {
            if (is_inside_subnet(subnet, addr)) {
                if (!closest_subnet.has_value()) {
                    closest_subnet = subnet;
                    closest_route = route;

                } else if (subnet.no_mask_bits > closest_subnet.value().no_mask_bits) {
                    closest_subnet = subnet;
                    closest_route = route;
                    
                }
            }
        }

        return closest_route;
    }
};