#include "ptp_header.h"

#include "lib/split_str.h"

#include <iostream>

int write_in_network_order(uint8_t* dest, ptp_action const* src) {
    uint8_t* curr = dest;

    *curr++ = static_cast<uint8_t>(src->type);

    switch(src->type) {
        case ptp_action_t::endpoint: {
            curr += write_in_network_order(curr, &std::get<ptp_endpoint>(src->val));
            break;
        }
        case ptp_action_t::local_conn: {
            curr += write_in_network_order(curr, &std::get<ptp_local_conn>(src->val));
            break;
        }
        case ptp_action_t::net_switch: {
            curr += write_in_network_order(curr, &std::get<ptp_net_switch>(src->val));
            break;
        }
        default: throw;
    }

    return curr - dest;
}

int extract_from_network_order(ptp_action* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    dest->type = static_cast<ptp_action_t>(*curr++);

    switch(dest->type) {
        case ptp_action_t::endpoint: {
            dest->val = ptp_endpoint();
            curr += extract_from_network_order(&std::get<ptp_endpoint>(dest->val), curr);
            break;
        }
        case ptp_action_t::local_conn: {
            dest->val = ptp_local_conn();
            curr += extract_from_network_order(&std::get<ptp_local_conn>(dest->val), curr);
            break;
        }
        case ptp_action_t::net_switch: {
            dest->val = ptp_net_switch();
            curr += extract_from_network_order(&std::get<ptp_net_switch>(dest->val), curr);
            break;
        }
        default: throw;
    }

    return curr - src;
}


optional<string> ptp_action_to_str(ptp_action const& action) {
    switch (action.type)
    {
    case ptp_action_t::endpoint: {
        auto tmp = ptp_endpoint_to_str(std::get<ptp_endpoint>(action.val));
        if (!tmp.has_value()) return {};

        return "endpoint " + tmp.value();
    }
    case ptp_action_t::local_conn: return "conn " + ptp_local_conn_to_str(std::get<ptp_local_conn>(action.val));
    case ptp_action_t::net_switch: return "switch " + ptp_net_switch_to_str(std::get<ptp_net_switch>(action.val));
    default: return {};
    }
}

optional<ptp_action> str_to_ptp_action(string const& str) {
    if (str.rfind("endpoint ", 0) != std::string::npos) {
        auto tmp = str_to_ptp_endpoint(str.substr(9));
        if (!tmp.has_value()) return {};

        return ptp_action{ptp_action_t::endpoint, tmp.value()};

    } else if (str.rfind("conn ", 0) != std::string::npos) {
        auto tmp = str_to_ptp_local_conn(str.substr(5));
        if (!tmp.has_value()) return {};

        return ptp_action{ptp_action_t::local_conn, tmp.value()};

    } else if (str.rfind("switch ", 0) != std::string::npos) {
        auto tmp = str_to_ptp_net_switch(str.substr(7));
        if (!tmp.has_value()) return {};

        return ptp_action{ptp_action_t::net_switch, tmp.value()};

    } else return {};
}


int write_in_network_order(uint8_t* dest, ptp_header const* src) {
    uint8_t* curr = dest;

    if (!(src->actions.size() < 256)) throw "too many actions";
    *curr++ = (uint8_t) src->actions.size();

    for (int i = 0; i < src->actions.size(); i++)
        curr += write_in_network_order(curr, &src->actions[i]);

    *curr++ = src->curr_action;

    *curr++ = src->no_jumps;

    return curr - dest;
}

int extract_from_network_order(ptp_header* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    uint8_t no_actions = *curr++;
    dest->actions.resize(no_actions);

    for (int i = 0; i < no_actions; i++)
        curr += extract_from_network_order(&dest->actions[i], curr);

    dest->curr_action = *curr++;

    dest->no_jumps = *curr++;

    return curr - src;
}


optional<string> ptp_action_list_to_str(ptp_action_list const& list) {
    string res;

    for (int i = 0; i < list.size(); i++) {
        if (i > 0) res += " ; ";
        
        if (auto tmp = ptp_action_to_str(list[i]); tmp.has_value()) res += tmp.value();
        else return {};
    }

    return res;
}

optional<ptp_action_list> str_to_ptp_action_list(string const& str) {
    ptp_action_list list;

    for (auto const& action_str : split_str(str, " ; ")) {
        auto tmp = str_to_ptp_action(action_str);
        if (!tmp.has_value()) return {};

        list.push_back(tmp.value());
    }

    return list;
}


ptp_endpoint create_reverse_action(ptp_endpoint const& endpoint) {
    if (endpoint.type == ptp_endpoint_t::start) return ptp_endpoint{ptp_endpoint_t::end};
    else if (endpoint.type == ptp_endpoint_t::end) return ptp_endpoint{ptp_endpoint_t::start};
    else throw;
}

ptp_local_conn create_reverse_action(ptp_local_conn const& local_conn) {
    return ptp_local_conn{local_conn.dest_addr, local_conn.src_addr};
}

ptp_net_switch create_reverse_action(ptp_net_switch const& net_switch) {
    return ptp_net_switch{net_switch.dest_id, net_switch.src_id};
}

ptp_action create_reverse_action(ptp_action const& action) {
    switch (action.type)
    {
    case ptp_action_t::endpoint:
        return {ptp_action_t::endpoint, create_reverse_action(std::get<ptp_endpoint>(action.val))};
    case ptp_action_t::local_conn:
        return {ptp_action_t::local_conn, create_reverse_action(std::get<ptp_local_conn>(action.val))};
    case ptp_action_t::net_switch:
        return {ptp_action_t::net_switch, create_reverse_action(std::get<ptp_net_switch>(action.val))};
    default: throw "err ptp_action_t";
    }
}

ptp_action_list create_reverse_actions(ptp_action_list const& actions) {
    ptp_action_list res;

    for (int i = actions.size() - 1; i >= 0; i--)
        res.push_back(create_reverse_action(actions[i]));

    return res;
}


bool ptp_is_endpoint_end(ptp_header const& header) {
    if (header.curr_action != header.actions.size() - 1) return false;

    ptp_action const& action = header.get_curr_action();
    if (action.type != ptp_action_t::endpoint) return false;


    ptp_endpoint endpoint = std::get<ptp_endpoint>(action.val);
    if (endpoint.type != ptp_endpoint_t::end) return false;

    return true;
}
