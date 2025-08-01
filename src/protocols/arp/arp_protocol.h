#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <cstdint>
#include <optional>
#include <functional>
#include <map>
using namespace std;

#include "src/abstract/network_access/net_access_bytes.h"

#include "arp_header.h"

namespace arp_protocol {
    void send(shared_ptr<net_access_bytes> const& net_access, arp_header const& header);

    void connect_recv(
        shared_ptr<net_access_bytes> const& net_access, shared_ptr<basic_recv_listener<arp_header>> const& recv,
        optional<arp_op_values> op
    );

}
