#pragma once

#include <optional>
#include <string>
using namespace std;

optional<int> open_free_bpf();

optional<int> open_bpf_iface(string const& iface_name);