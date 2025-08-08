#include "virtual_net.h"

optional<int> lock_file(fs::path const& file) {
    auto fd = open(file.c_str(), O_CREAT | O_RDWR, 0600);
    if (fd == OPEN_ERROR) return {};

    if (flock(fd, LOCK_EX) == FLOCK_ERROR) {
        close(fd);
        return {};
    }

    return fd;
}

void unlock_file(int fd) {
    flock(fd, LOCK_UN);
    close(fd);
}