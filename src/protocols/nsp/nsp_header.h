#pragma once

#include <string>
#include <optional>
using namespace std;

#include "nsp_id.h"

struct nsp_header {
    nsp_id src_id;
    nsp_id dest_id;
};

int write_in_network_order(uint8_t* dest, nsp_header const* src);

int extract_from_network_order(nsp_header* dest, uint8_t const* src);
