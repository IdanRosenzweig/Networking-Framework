#pragma once

#include "basic_recv_listener.h"

#include <utility>
#include <memory>
#include <vector>
using namespace std;

template <typename PROT>
using encap_prot_listener =
basic_recv_listener<
    pair<shared_ptr<PROT>, vector<uint8_t>>
>;