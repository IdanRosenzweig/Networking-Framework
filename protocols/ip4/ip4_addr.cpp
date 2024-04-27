#include "ip4_addr.h"
#include <arpa/inet.h>

ip4_addr convert_to_ip4_addr(const string &str) {
    int a, b, c, d;
    if (sscanf(str.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        throw "can't convert ip4 address";
    }

    ip4_addr addr;
    addr.octets[0] = a;
    addr.octets[1] = b;
    addr.octets[2] = c;
    addr.octets[3] = d;

    return addr;
}

void write_in_network_order(uint8_t *dest, ip4_addr *source) {
    dest[0] = source->octets[0];
    dest[1] = source->octets[1];
    dest[2] = source->octets[2];
    dest[3] = source->octets[3];
}

void extract_from_network_order(ip4_addr *dest, uint8_t *source) {
    dest->octets[0] = source[0];
    dest->octets[1] = source[1];
    dest->octets[2] = source[2];
    dest->octets[3] = source[3];
}