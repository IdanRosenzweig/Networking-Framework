#include "bpf.h"

#include <filesystem>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/bpf.h>

#include "error_codes.h"

optional<int> open_free_bpf() {
    static const filesystem::path dir("/dev");
    for (int i = 0; i < 100; i++) {
        filesystem::path entry = dir / (string("bpf") + to_string(i));

        int fd = open(entry.c_str(), O_RDWR);
        if (fd != OPEN_ERROR) return fd;
    }

    return {};
}

optional<int> open_bpf_iface(string const& iface_name) {
    auto fd = open_free_bpf();
    if (!fd.has_value()) return {};

    struct ifreq ifr;
    strncpy(ifr.ifr_name, iface_name.c_str(), sizeof(ifr.ifr_name) - 1);
    ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
    if (ioctl(fd.value(), BIOCSETIF, &ifr) == IOCTL_ERROR) {close(fd.value()); return {};}

    u_int enable = 1;
    if (ioctl(fd.value(), BIOCSHDRCMPLT, &enable) == IOCTL_ERROR) {close(fd.value()); return {};}

    return fd;
}

