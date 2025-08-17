#pragma once

#include "ptp_addr.h"

#include "src/abstract/routing/routing_table.h"

struct ptp_local_conn_routing_table : public routing_table<ptp_subnet_mask, ptp_addr> {
private:
    vector<pair<ptp_subnet_mask, shared_ptr<net_access>>> rules;

protected:
    shared_ptr<net_access> impl_find_route(ptp_addr const& addr) const override {
        optional<ptp_subnet_mask> closest_subnet = {};
        shared_ptr<net_access> closest_route = nullptr;

        for (auto& [subnet, route] : rules) {
            if (is_inside_subnet(subnet, addr)) {
                if (!closest_subnet.has_value()) {
                    closest_subnet = subnet;
                    closest_route = route;

                } else if (subnet.prefix.octets.size() > closest_subnet.value().prefix.octets.size()) {
                    closest_subnet = subnet;
                    closest_route = route;
                    
                }
            }
        }

        return closest_route;
    }

public:
    void add_routing_rule(ptp_subnet_mask const& subnet, shared_ptr<net_access>&& net_access) override {
        rules.push_back({subnet, std::move(net_access)});
    }
};