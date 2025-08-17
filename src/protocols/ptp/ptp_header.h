#pragma once

#include <cstdint>
#include <vector>
#include <variant>
using namespace std;

#include "endpoints/ptp_endpoint.h"
#include "local_conn/ptp_local_conn.h"
#include "net_switch/ptp_net_switch.h"

enum class ptp_action_t : uint8_t {
    endpoint, // path endpoint
    local_conn, // a connection between nodes in the same network
    net_switch // switching to another network
};
struct ptp_action {
    ptp_action_t type;
    variant<ptp_endpoint, ptp_local_conn, ptp_net_switch> val;
};

int write_in_network_order(uint8_t* dest, ptp_action const* src);

int extract_from_network_order(ptp_action* dest, uint8_t const* src);

optional<string> ptp_action_to_str(ptp_action const& action);

optional<ptp_action> str_to_ptp_action(string const& str);


using ptp_action_list = vector<ptp_action>;
struct ptp_header {
    ptp_action_list actions;
    uint8_t curr_action;

    ptp_action const& get_curr_action() const {
        if (!(curr_action < actions.size())) throw;
        return actions[curr_action];
    }

    uint8_t no_jumps;

    // prot
};

int write_in_network_order(uint8_t* dest, ptp_header const* src);

int extract_from_network_order(ptp_header* dest, uint8_t const* src);

optional<string> ptp_action_list_to_str(ptp_action_list const& list);

optional<ptp_action_list> str_to_ptp_action_list(string const& str);

ptp_action_list create_reverse_actions(ptp_action_list const& actions);

bool ptp_is_endpoint_end(ptp_header const& header);