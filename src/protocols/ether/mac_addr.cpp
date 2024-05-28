#include "mac_addr.h"
#include <stdio.h>

mac_addr convert_to_mac_addr(const std::string &str) {
    mac_addr addr;
    sscanf(str.c_str(), "%x:%x:%x:%x:%x:%x",
           (unsigned int *) &addr.octets[0],
           (unsigned int *) &addr.octets[1],
           (unsigned int *) &addr.octets[2],
           (unsigned int *) &addr.octets[3],
           (unsigned int *) &addr.octets[4],
           (unsigned int *) &addr.octets[5]
    );

    return addr;
}

std::string convert_to_str(mac_addr addr) {
    char buff[2 * 6 + 5 + 1] = {0};
    sprintf(buff, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned int) addr.octets[0],
            (unsigned int) addr.octets[1],
            (unsigned int) addr.octets[2],
            (unsigned int) addr.octets[3],
            (unsigned int) addr.octets[4],
            (unsigned int) addr.octets[5]);

    return std::string(buff);
}
