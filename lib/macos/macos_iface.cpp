#include "macos_iface.h"
#include "error_codes.h"

#include "bpf.h"

#include <unistd.h>

#include <iostream>
#include <utility>
#include <cstring>
using namespace std;


void my_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    macos_iface *class_ptr = reinterpret_cast<macos_iface *>(user);

    int data_sz = pkthdr->caplen;
    if (data_sz <= 0) {
        return;
    }

    // the data
    vector<uint8_t> data((uint8_t*) bytes, ((uint8_t*) bytes) + data_sz);

    // forward to the recv access
    class_ptr->async_recv->handle_recv(data);

    // send to sniff
    if (class_ptr->sniff != nullptr)
        class_ptr->sniff->handle_recv(data);
}


macos_iface::macos_iface(string const& iface) : iface_name(iface), async_recv(make_shared<async_recv_listener<vector<uint8_t>>>()) {
    // recv access
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        traffic = pcap_open_live(iface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic == nullptr) {
            std::cerr << "can't open interface: " << iface << ", err: " << errbuf << endl;
            throw;
        }

        worker_traffic = std::thread([this]() {
            pcap_loop(traffic, -1, my_handler, reinterpret_cast<u_char *>(this));
        });
    }

    std::this_thread::sleep_for(20ms); // ensure the pcap_loops have started // todo fix this

    // setup the send access
    if (auto tmp = open_bpf_iface(iface); tmp.has_value()) fd = tmp.value();
    else {
        std::cerr << "fd err" << endl;
        printf("errno: %d\n", errno);
        throw;
    }

    struct my_send : public send_medium_bytes {
        int fd;
        my_send(int fd) : fd(fd) {}

        err_t send_data(vector<uint8_t> const& data) override {
            int res = write(fd,
                data.data(), data.size()
            );

            if (res == SEND_ERROR || res == 0) return err_t::ERR;
            else return err_t::OK;
        }
    };
    send_access = make_shared<my_send>(fd);
}

macos_iface::~macos_iface() {
    if (worker_traffic.joinable()) worker_traffic.detach();
}

