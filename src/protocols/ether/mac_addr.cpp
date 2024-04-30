//#include <linux/if.h>
//#include <cstring>
//#include <cstdio>
//#include <linux/if_ether.h>
//#include <sys/ioctl.h>
//#include <unistd.h>
//#include <netinet/in.h>
//#include "mac_addr.h"
//
//mac_addr get_my_mac_address(const char *interface_name) {
//    struct ifreq ifr;
//    memset(&ifr, 0, sizeof ifr);
//    snprintf(ifr.ifr_name, IFNAMSIZ, "%s", interface_name);
//
//    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
//    if (ioctl(temp_fd, SIOCGIFHWADDR, &ifr) < 0)
//        return {0};
//    close(temp_fd);
//
//    mac_addr octets;
//    memset(&octets, 0, sizeof(octets));
//    memcpy(&octets, ifr.ifr_hwaddr.sa_data, sizeof(octets));
//
//    return octets;
//}
//
//void print_mac(mac_addr octets) {
//    printf("mac octets is: %02x:%02x:%02x:%02x:%02x:%02x\n",
//           (unsigned int) octets.octets[0],
//           (unsigned int) octets.octets[1],
//           (unsigned int) octets.octets[2],
//           (unsigned int) octets.octets[3],
//           (unsigned int) octets.octets[4],
//           (unsigned int) octets.octets[5]
//    );
//}
//
//ip4_addr get_my_priv_ip_addr(const char *interface) {
//    int temp_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
//
//    // Get the private ip address of our device
//    struct ifreq my_priv_ip;
//    memset(&my_priv_ip, 0, sizeof(struct ifreq));
//    strncpy(my_priv_ip.ifr_name, interface, IFNAMSIZ - 1);
//    if (ioctl(temp_fd, SIOCGIFADDR, &my_priv_ip) < 0)
//        perror("SIOCGIFADDR");
//
//    close(temp_fd);
//
//    return {
//            ((struct sockaddr_in *) &my_priv_ip.ifr_addr)->sin_addr.s_addr
//    };
//}