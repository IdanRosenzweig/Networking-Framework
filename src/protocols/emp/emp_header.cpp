#include "emp_header.h"

#include <netinet/in.h>

#include "lib/var_len_encoding.h"

int write_in_network_order(uint8_t* dest, emp_header const* src) {
    uint8_t* curr = dest;

    // src and dest ports
    curr += write_in_network_order(curr, &src->src_port);
    curr += write_in_network_order(curr, &src->dest_port);

    return curr - dest;
}

int extract_from_network_order(emp_header* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    // src and dest ports
    curr += extract_from_network_order(&dest->src_port, curr);
    curr += extract_from_network_order(&dest->dest_port, curr);

    return curr - src;
}