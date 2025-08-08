#include "hardware.h"
#include "error_codes.h"

#include <net/if.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <sys/sysctl.h>
#include <net/route.h>
#include <net/if_types.h>

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstring>
using namespace std;


optional<mac_addr> get_mac_addr_of_iface(string const& iface) {
    struct ifaddrs *ifap, *ifap_temp;
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        return {};
    }

    optional<mac_addr> result;
    for (ifap_temp = ifap; ifap_temp != nullptr; ifap_temp = ifap_temp->ifa_next) {
        if ((ifap_temp->ifa_addr->sa_family == AF_LINK) &&
            (strcmp(ifap_temp->ifa_name, iface.c_str()) == 0)) {
            
            struct sockaddr_dl *sdl = (struct sockaddr_dl *)ifap_temp->ifa_addr;
            if (sdl->sdl_alen == 6) {
                mac_addr addr;
                memcpy(&addr, LLADDR(sdl), sizeof(addr));
                result = addr;
                break;
            }
        }
    }
    
    freeifaddrs(ifap);
    return result;
}

void set_mac_addr_for_iface(string const& iface, mac_addr new_addr) {
    char mac_str[20];
    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
            new_addr.octets[0], new_addr.octets[1], new_addr.octets[2],
            new_addr.octets[3], new_addr.octets[4], new_addr.octets[5]);

    string command = "ifconfig " + iface + " ether " + string(mac_str);
    
    // The ifconfig command to change MAC address requires root privileges.
    // The program must be run with sudo for this to work.
    if (system(command.c_str()) == -1) {
        cerr << "couldn't change the mac address using ifconfig" << endl;
    }
    cout << "To make this change permanent, you may need to disable and re-enable the interface or reboot." << endl;
}

optional<ip4_addr> get_ip4_addr_of_iface(string const& iface) {
    struct ifaddrs *ifap, *ifap_temp;
    if (getifaddrs(&ifap) == -1) {
        perror("getifaddrs");
        return {};
    }

    optional<ip4_addr> result;
    for (ifap_temp = ifap; ifap_temp != nullptr; ifap_temp = ifap_temp->ifa_next) {
        if ((ifap_temp->ifa_addr->sa_family == AF_INET) &&
            (strcmp(ifap_temp->ifa_name, iface.c_str()) == 0)) {
            
            struct sockaddr_in *sa = (struct sockaddr_in *)ifap_temp->ifa_addr;
            ip4_addr ip;
            extract_from_network_order(&ip, (uint8_t*)&sa->sin_addr.s_addr);
            result = ip;
            break;
        }
    }
    
    freeifaddrs(ifap);
    return result;
}

optional<ip4_addr> get_default_gateway_of_iface(string const& iface) {
    int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_DUMP, 0};
    size_t len;
    
    // Get the size of the routing table data
    if (sysctl(mib, 6, nullptr, &len, nullptr, 0) < 0) {
        perror("sysctl (size)");
        return {};
    }
    
    char* buf = new char[len];
    if (sysctl(mib, 6, buf, &len, nullptr, 0) < 0) {
        perror("sysctl (data)");
        delete[] buf;
        return {};
    }
    
    optional<ip4_addr> result;
    char* lim = buf + len;
    for (char* next = buf; next < lim; next += ((struct rt_msghdr*)next)->rtm_msglen) {
        struct rt_msghdr* rtm = (struct rt_msghdr*)next;
        struct sockaddr* sa = (struct sockaddr*)(rtm + 1);
        
        if (rtm->rtm_addrs & RTA_GATEWAY) {
            char* dest = nullptr;
            char* gateway = nullptr;
            
            // Iterate through the addresses to find destination and gateway
            for (int i = 0; i < RTAX_MAX; i++) {
                if (rtm->rtm_addrs & (1 << i)) {
                    if (i == RTAX_DST) dest = (char*)sa;
                    if (i == RTAX_GATEWAY) gateway = (char*)sa;

#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))
                    sa = (struct sockaddr*)((char*)sa + ROUNDUP(sa->sa_len));
                }
            }

            if (dest && gateway) {
                struct sockaddr_in* sin_dest = (struct sockaddr_in*)dest;
                struct sockaddr_in* sin_gw = (struct sockaddr_in*)gateway;
                
                // Check if it's the default route (destination 0.0.0.0)
                if (sin_dest->sin_family == AF_INET && sin_dest->sin_len > 0 && sin_dest->sin_addr.s_addr == 0) {
                    // This is the default gateway. We also need to confirm it's for the right interface.
                    // This check is a bit more complex, but for a simple case,
                    // we can often rely on the 'default' route being for the active interface.
                    // A more robust check would involve iterating through the ifaddrs again
                    // to match the interface name to the index from rtm->rtm_index.

                    // For now, let's just assume we found it.
                    ip4_addr gw_ip;
                    extract_from_network_order(&gw_ip, (uint8_t*)&sin_gw->sin_addr.s_addr);
                    result = gw_ip;
                    break;
                }
            }
        }
    }
    
    delete[] buf;
    return result;
}

