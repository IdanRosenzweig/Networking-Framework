#pragma once

#include <string>
#include <optional>
using namespace std;

#include "emp_port.h"

struct emp_header {
    emp_port src_port;
    emp_port dest_port;
};

int write_in_network_order(uint8_t* dest, emp_header const* src);

int extract_from_network_order(emp_header* dest, uint8_t const* src);
