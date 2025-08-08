#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include <optional>
#include <functional>
#include <map>
using namespace std;

#include "src/abstract/net_access/net_access.h"

#include "icmp_header.h"

namespace icmp_protocol {
    void send(shared_ptr<net_access> const& icmp_surface, icmp_header const& header, vector<uint8_t> const& data);

    void connect_recv(
        shared_ptr<net_access> const& icmp_surface, shared_ptr<recv_listener<pair<icmp_header, vector<uint8_t>>>> const& recv,
        optional<icmp_type> type
    );
}

// class icmp_protocol : public msg_send_medium, public msg_recv_listener {
// public:

//     icmp_protocol();

//     // send
//     next_choice<int> next_type;
//     next_choice<int> next_code;
//     next_choice<uint32_t> next_content;
//     msg_send_forwarder send_medium;

//     int send_data(send_msg_t &&data) override;


//     // recv
// #define ICMP_LISTEN_ON_TYPE_INDEX 0
// #define ICMP_LISTEN_ON_CODE_INDEX 1
//     multiplexer<recv_listener *,
//             uint8_t, // type, index 0
//             uint8_t // code, index 1
//     > listeners;
//     recv_listener *default_listener = nullptr;

//     void handle_callback(recv_msg_t &&data) override;

// };


