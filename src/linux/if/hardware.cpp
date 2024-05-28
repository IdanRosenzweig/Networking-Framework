#include "hardware.h"
#include "../error_codes.h"

#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <cstring>
#include <cstdio>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;

mac_addr get_mac_addr_of_iface(const string &iface) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof ifr);
    snprintf(ifr.ifr_name, IFNAMSIZ, "%s", iface.c_str());

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ioctl(temp_fd, SIOCGIFHWADDR, &ifr) == IOCTL_ERROR)
        return {0};
    close(temp_fd);

    mac_addr addr;
    memset(&addr, 0, sizeof(addr));
    memcpy(&addr, ifr.ifr_hwaddr.sa_data, sizeof(addr));

    return addr;
}

void set_mac_addr_for_iface(const string &iface, mac_addr new_addr) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof ifr);
    snprintf(ifr.ifr_name, IFNAMSIZ, "%s", iface.c_str());
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, &new_addr, sizeof(new_addr));

    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (ioctl(temp_fd, SIOCSIFHWADDR, &ifr) == IOCTL_ERROR) {
        cerr << "couldn't change the mac address inside the kernel using ioctl" << endl;
    }
    close(temp_fd);

}

ip4_addr get_ip_addr_of_iface(const string &iface) {
    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    // Get the private ip address of our device
    struct ifreq my_priv_ip;
    memset(&my_priv_ip, 0, sizeof(struct ifreq));
    strncpy(my_priv_ip.ifr_name, iface.c_str(), IFNAMSIZ - 1);
    if (ioctl(temp_fd, SIOCGIFADDR, &my_priv_ip) == IOCTL_ERROR)
        perror("SIOCGIFADDR");

    close(temp_fd);

    ip4_addr ip;
    extract_from_network_order(&ip,
                               (uint8_t*) &((struct sockaddr_in *) &my_priv_ip.ifr_addr)->sin_addr.s_addr);
    return ip;
}

ip4_addr get_default_gateway_of_iface(const string &iface) {
#define ROUTE_FILE "/proc/net/route"
    FILE *fp = fopen(ROUTE_FILE, "r");
    if (fp == nullptr) {
        std::cerr << "can't open " ROUTE_FILE << std::endl;
        throw;
    }

    // Skip the first line (header)
    char buffer[2000];
    if (fgets(buffer, sizeof(buffer), fp) == nullptr) {
        std::cerr << "invalid header of " ROUTE_FILE << std::endl;
        throw;
    }

    char curr_iface[IFNAMSIZ];
    uint32_t dest, gw;
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        memset(curr_iface, 0, IFNAMSIZ);
        int res = sscanf(buffer, "%s %x %x", curr_iface, &dest, &gw);
        if (res != 3) continue;
        if (strcmp(curr_iface, iface.c_str()) != 0 || dest != 0) continue;

        ip4_addr gw_ip;
        extract_from_network_order(&gw_ip, (uint8_t*) &gw);

        fclose(fp);
        return gw_ip;
    }

    fclose(fp);
    return empty_ip4_addr;

}
