#pragma once

#include <string>
#include <utility>
using namespace std;

// opens a TUN/TAP interface
pair<int, string> open_tun_tap(string const& iface_name, int flags);

// opens a raw TAP interface (wrapper of open_tun_tap)
pair<int, string> open_raw_tap(string const& iface_name);

void set_up_tun_tap(string const& iface_name);
void set_down_tun_tap(string const& iface_name);

