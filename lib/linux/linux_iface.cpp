#include "linux_iface.h"
#include "error_codes.h"

#include <linux/if.h>
#include <linux/if_ether.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>

#include <iostream>
#include <utility>
#include <cstring>
using namespace std;


void my_handler(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    linux_iface *class_ptr = reinterpret_cast<linux_iface *>(user);

    int data_sz = pkthdr->caplen;
    if (data_sz <= 0) {
        return;
    }

    // the data
    vector<uint8_t> data((uint8_t*) bytes, ((uint8_t*) bytes) + data_sz);

    // forward to the recv access
    class_ptr->async_recv->handle_recv(data);

    // send to incoming sniff
    if (class_ptr->sniff != nullptr)
        class_ptr->sniff->handle_recv(data);
}

linux_iface::linux_iface(string const& iface) : iface_name(iface), async_recv(make_shared<async_recv_listener<vector<uint8_t>>>()) {
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

    // send access
    fd = socket(
            AF_PACKET,
            SOCK_RAW,
            htons(ETH_P_ALL)
    );
    if (fd == OPEN_ERROR) {
        std::cerr << "fd err" << endl;
        printf("errno: %d\n", errno);
        throw;
    }

    struct ifreq if_idx;
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, iface.c_str(), IFNAMSIZ - 1);

    if (ioctl(fd, SIOCGIFINDEX, &if_idx) == IOCTL_ERROR)
        perror("SIOCGIFINDEX");

    struct sockaddr_ll outputServer;
    memset(&outputServer, 0, sizeof(struct sockaddr_ll));

    outputServer.sll_family = AF_PACKET;
    outputServer.sll_protocol = htons(ETH_P_ALL);
    outputServer.sll_ifindex = if_idx.ifr_ifindex;
    outputServer.sll_halen = ETH_ALEN;

    if (bind(fd, (struct sockaddr *) &outputServer, sizeof(outputServer)) == BIND_ERROR) {
        perror("bind");
        throw;
    }

    struct my_send : public send_medium_bytes {
        int fd;
        my_send(int fd) : fd(fd) {}

        err_t send_data(vector<uint8_t> const& data) override {
            int res = send(fd,
                data.data(), data.size(),
                0);

            if (res == SEND_ERROR || res == 0) return err_t::ERR;
            else return err_t::OK;
        }
    };
    send_access = make_shared<my_send>(fd);
}

linux_iface::~linux_iface() {
    if (worker_traffic.joinable()) worker_traffic.detach();
}

