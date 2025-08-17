#include "nsp_header.h"

#include <netinet/in.h>

#include "lib/var_len_encoding.h"

int write_in_network_order(uint8_t* dest, nsp_header const* src) {
    uint8_t* curr = dest;

    // src and dest ids
    curr += write_in_network_order(curr, &src->src_id);
    curr += write_in_network_order(curr, &src->dest_id);

    return curr - dest;
}

int extract_from_network_order(nsp_header* dest, uint8_t const* src) {
    uint8_t const* curr = src;

    // src and dest ids
    curr += extract_from_network_order(&dest->src_id, curr);
    curr += extract_from_network_order(&dest->dest_id, curr);

    return curr - src;
}