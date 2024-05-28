#include "virtual_if.h"
#include "../../error_codes.h"

#include "tun_tap.h"
#include <unistd.h>
#include <iostream>
using namespace std;

linux_virtual_iface::linux_virtual_iface(gateway *gw, string& iface_name) : base_gw(gw) {
    base_gw->add_listener(this);

    fd = open_raw_tap(iface_name);
    if (fd == OPEN_ERROR) {
        std::cerr << "can't create virual tap interface" << endl;
        throw;
    }

    set_up_tun_tap(iface_name);

    worker = std::thread([&]() -> void {
        while (true) {

            send_msg<> send;
            uint8_t *buff = send.get_active_buff();

            int cnt = read(fd, buff, 1024);
            if (cnt <= 0) continue;

//            std::cout << "tap sending " << cnt << " bytes" << endl;
            send.set_count(cnt);
            this->base_gw->send_data(std::move(send));
        }
    });
}

linux_virtual_iface::~linux_virtual_iface() {
    base_gw->remove_listener(this);
    worker.detach();
}

void linux_virtual_iface::handle_received_event(received_msg &&event) {
    int cnt = event.data.size() - event.curr_offset;
//    std::cout << "tap received " << cnt << " bytes" << endl;
    write(fd, event.data.data() + event.curr_offset, event.data.size() - event.curr_offset);
//    write(fd, event.data_t.get(), event.data.size());
}



