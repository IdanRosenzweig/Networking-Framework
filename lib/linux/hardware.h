#pragma once

#include "src/protocols/ether2/mac_addr.h"
#include "src/protocols/ip4/ip4_addr.h"

#include <optional>
using namespace std;

optional<mac_addr> get_mac_addr_of_iface(string const& iface);

void set_mac_addr_for_iface(string const& iface, mac_addr new_addr);

optional<ip4_addr> get_ip4_addr_of_iface(string const& iface);

optional<ip4_addr> get_default_gateway_of_iface(string const& iface);
