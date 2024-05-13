#include "virtual_if.h"

#include "tun_tap.h"
#include <unistd.h>

linux_virtual_iface::linux_virtual_iface(basic_gateway *ggw, char *dev) : gateway(ggw) {
    gateway->add_listener(this);

    fd = open_raw_tap(dev);
    if (fd == -1)
        throw "can't create virual tap interface";

    set_up_tun_tap(dev);

    worker = std::thread([&]() -> void {
        while (true) {
#define BUFF_SZ 1024
            char buff[BUFF_SZ];
            memset(buff, 0, BUFF_SZ);

            int cnt = read(fd, buff, BUFF_SZ);
            if (cnt <= 0) continue;

            cout << "tap sending " << cnt << " bytes" << endl;
            this->gateway->send_data({buff, cnt});
        }
    });
}

void linux_virtual_iface::handle_received_event(received_msg &event) {
    int cnt = event.sz - event.curr_offset;
//    cout << "tap received " << cnt << " bytes" << endl;
    write(fd, event.data.get() + event.curr_offset, event.sz - event.curr_offset);
//    write(fd, event.data.get(), event.sz);
}

linux_virtual_iface::~linux_virtual_iface() {
    if (worker.joinable()) worker.detach();
    cout << "virtual iface decosntrucotr" << endl;
}

linux_virtual_iface create_virtual_iface_from_connection(basic_gateway *ggw, char dev[6]) {
    return linux_virtual_iface(ggw, dev);
}

