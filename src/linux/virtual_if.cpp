#include "virtual_if.h"

#include "tun_tap.h"
#include <unistd.h>
#include <iostream>
using namespace std;

linux_virtual_iface::linux_virtual_iface(msg_gateway *gw, char *dev) : gateway(gw) {
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

//            cout << "tap sending " << cnt << " bytes" << endl;
            send_msg send{buff, cnt};
            this->gateway->send_data(send);
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
}

