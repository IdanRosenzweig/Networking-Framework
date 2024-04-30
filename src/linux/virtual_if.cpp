#include "virtual_if.h"

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>

int linux_create_tap(char *dev) {
#define CLONE_PATH "/dev/net/tun"

    int fd;
    if( (fd = open(CLONE_PATH , O_RDWR)) < 0 ) {
        perror("Opening /dev/net/tun");
        return fd;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_flags = IFF_TAP; // tap

    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    int err;
    if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
        perror("ioctl");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);

    return fd;
}

linux_virtual_iface::linux_virtual_iface(basic_gateway *gateway, char *dev) : gateway(gateway) {
    gateway->add_listener(this);

    fd = linux_create_tap(dev);
    if (fd == -1) {
        throw "can't create virual tap interface";
    }

    string conf_command = "sudo ip link set dev " + string(dev) + " up";
    system(conf_command.c_str());

    worker = std::thread([&]() -> void {
        while (true) {
#define BUFF_SZ 1024
            char buff[BUFF_SZ];
            memset(buff, 0, BUFF_SZ);

            int cnt = read(fd, buff, BUFF_SZ);
            if (cnt <= 0) continue;

            gateway->send_data({buff, cnt});
        }
    });
}

linux_virtual_iface::~linux_virtual_iface() {
    worker.detach();
}

void linux_virtual_iface::handle_received_event(received_msg &event) {
    write(fd, event.data.get() + event.curr_offset, event.sz - event.curr_offset);
}
