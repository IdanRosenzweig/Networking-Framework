#include "internet_checksum.h"

//unsigned short internet_checksum(const unsigned short * buf, int no_bytes) {
//    unsigned long sum;
//    for (sum = 0; no_bytes >= 2; no_bytes -= 2) {
//        sum += *buf;
//        buf++;
//    }
//    if (no_bytes == 1)
//        sum += *reinterpret_cast<const unsigned char*>(buf);
//
//    sum = (sum >> 16) + (sum & 0xffff);
//    sum += (sum >> 16);
//    return ~sum;
//}

unsigned short internet_checksum(const uint16_t * buf, int no_bytes) {
    int len = no_bytes;
    auto *current = reinterpret_cast<const uint16_t *>(buf);

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