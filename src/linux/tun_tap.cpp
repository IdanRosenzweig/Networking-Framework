#include "tun_tap.h"

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <cstring>
#include <string>

int open_tun_tap(char *dev, int flags) {
#define CLONE_PATH "/dev/net/tun"

    int fd;
    if ((fd = open(CLONE_PATH, O_RDWR)) < 0) {
        throw "can't open " CLONE_PATH;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = flags;
    if (*dev != 0) strncpy(ifr.ifr_name, dev, IFNAMSIZ);


    if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
        close(fd);
        throw "error calling ioctl";
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}

int open_raw_tap(char *dev) {
    return open_tun_tap(dev, IFF_TAP | IFF_NO_PI);
}


void set_up_tun_tap(char *dev) {
    std::string conf_command = "sudo ip link set dev ";
    conf_command += dev;
    conf_command += " up";
    system(conf_command.c_str());
}
void set_down_tun_tap(char *dev) {
    std::string conf_command = "sudo ip link set dev ";
    conf_command += dev;
    conf_command += " down";
    system(conf_command.c_str());
}