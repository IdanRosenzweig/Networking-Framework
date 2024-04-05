#include <linux/if.h>
#include <cstring>
#include <cstdio>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include "mac_addr.h"

mac_addr get_my_mac_address(const char *interface_name) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof ifr);
    snprintf(ifr.ifr_name, IFNAMSIZ, "%s", interface_name);

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ioctl(temp_fd, SIOCGIFHWADDR, &ifr) < 0)
        return {0};
    close(temp_fd);

    mac_addr addr;
    memset(&addr, 0, sizeof(addr));
    memcpy(&addr, ifr.ifr_hwaddr.sa_data, sizeof(addr));

    return addr;
}

void print_mac(mac_addr addr) {
    printf("mac addr is: %02x:%02x:%02x:%02x:%02x:%02x\n",
           (unsigned int) addr.addr[0],
           (unsigned int) addr.addr[1],
           (unsigned int) addr.addr[2],
           (unsigned int) addr.addr[3],
           (unsigned int) addr.addr[4],
           (unsigned int) addr.addr[5]
    );
}