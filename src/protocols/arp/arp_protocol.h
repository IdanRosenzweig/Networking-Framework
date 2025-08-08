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

#include "arp_header.h"

namespace arp_protocol {
    void send(shared_ptr<net_access> const& net_access, arp_header const& header);

    void connect_recv(
        shared_ptr<net_access> const& net_access, shared_ptr<recv_listener<arp_header>> const& recv,
        optional<arp_op_values> op
    );

}
