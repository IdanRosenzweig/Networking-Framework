#ifndef NETWORKING_INTERNET_CHECKSUM_H
#define NETWORKING_INTERNET_CHECKSUM_H

#include <stdint.h>

uint16_t internet_checksum(const uint16_t * buf, int no_bytes);

#endif //NETWORKING_INTERNET_CHECKSUM_H
