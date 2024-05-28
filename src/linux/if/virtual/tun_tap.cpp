#include "tun_tap.h"
#include "../../error_codes.h"

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int open_tun_tap(std::string& iface_name, int flags) {
#define CLONE_PATH "/dev/net/tun"

    int fd;
    if ((fd = open(CLONE_PATH, O_RDWR)) == OPEN_ERROR) {
        std::cerr << "can't open " CLONE_PATH << std::endl;
        throw;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;
    if (!iface_name.empty()) strncpy(ifr.ifr_name, iface_name.c_str(), std::min((size_t) IFNAMSIZ, iface_name.size()));


    if (ioctl(fd, TUNSETIFF, (void *) &ifr) == IOCTL_ERROR) {
        close(fd);
        std::cerr << "error calling ioctl" << std::endl;
        throw;
    }

    iface_name = ifr.ifr_name;

    return fd;
}

int open_raw_tap(std::string& iface_name) {
    return open_tun_tap(iface_name, IFF_TAP | IFF_NO_PI);
}


void set_up_tun_tap(const std::string& iface_name) {
    std::string conf_command = "sudo ip link set dev ";
    conf_command += iface_name.c_str();
    conf_command += " up";
    system(conf_command.c_str());
}
void set_down_tun_tap(const std::string& iface_name) {
    std::string conf_command = "sudo ip link set dev ";
    conf_command += iface_name.c_str();
    conf_command += " down";
    system(conf_command.c_str());
}