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

            send_msg<> send;
            uint8_t *buff = send.get_active_buff();

            int cnt = read(fd, buff, 1024);
            if (cnt <= 0) continue;

//            cout << "tap sending " << cnt << " bytes" << endl;
            send.set_count(cnt);
            this->gateway->send_data(send);
        }
    });
}

void linux_virtual_iface::handle_received_event(received_msg &event) {
    int cnt = event.data.size() - event.curr_offset;
    cout << "tap received " << cnt << " bytes" << endl;
    write(fd, event.data.data() + event.curr_offset, event.data.size() - event.curr_offset);
//    write(fd, event.data_t.get(), event.data.size());
}

linux_virtual_iface::~linux_virtual_iface() {
    if (worker.joinable()) worker.detach();
}

