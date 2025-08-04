#include "internet_checksum.h"

uint16_t internet_checksum(uint8_t const* buf, int no_bytes) {
    int len = no_bytes;
    uint16_t const* current = reinterpret_cast<uint16_t const*>(buf);

    uint32_t sum = 0;
    while (len > 1) {
        sum += *current++;
        len -= 2;
    }
    if (len)
        sum += *reinterpret_cast<const uint8_t *>(current);

    sum = (sum >> 16u) + (sum & 0xFFFFu);
    sum += sum >> 16u;
    return ~sum;
}