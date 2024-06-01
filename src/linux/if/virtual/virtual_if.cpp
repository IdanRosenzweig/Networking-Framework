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

            send_msg_t send;
            uint8_t *buff = send.get_active_buff();

            int cnt = read(fd, buff, 1024);
            if (cnt <= 0) continue;

            send.set_count(cnt);
            this->base_gw->send_data(std::move(send));
        }
    });
}

linux_virtual_iface::~linux_virtual_iface() {
    base_gw->remove_listener(this);
    worker.detach();
}

void linux_virtual_iface::handle_callback(recv_msg_t &&data) {
    write(fd, data.buff_at_curr_offset(), data.buff_cnt_at_curr_offset());
}



