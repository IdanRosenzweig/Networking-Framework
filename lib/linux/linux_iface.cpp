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


void handler_out(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
    linux_iface *class_ptr = reinterpret_cast<linux_iface *>(user);

    int data_sz = pkthdr->caplen;
    if (data_sz <= 0) {
        return;
    }

    // the data
    vector<uint8_t> data((uint8_t*) bytes, ((uint8_t*) bytes) + data_sz);

    // send to outgoing sniff
    if (class_ptr->sniff_outgoing != nullptr)
        class_ptr->sniff_outgoing->handle_recv(data);
}

void handler_in(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *bytes) {
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
    if (class_ptr->sniff_incoming != nullptr)
        class_ptr->sniff_incoming->handle_recv(data);
}



// iface_access_point::iface_access_point(const string& iface) : iface_name(iface) {

//     // traffic out
//     outgoing_network_queue.add_listener(&outgoing_traffic);
//     {
//         char errbuf[PCAP_ERRBUF_SIZE];
//         traffic_out = pcap_open_live(iface.c_str(), BUFSIZ, 1, 10, errbuf);
//         if (traffic_out == nullptr) {
//             std::cerr << "can't open interface: " << iface << ", err: " << errbuf << endl;
//             throw;
//         }

//         const char *filter_exp = "outbound";

//         struct bpf_program fp;
//         if (pcap_compile(traffic_out, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
//             fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
//             return;
//         }

//         if (pcap_setfilter(traffic_out, &fp) == PCAP_ERROR) {
//             fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
//             return;
//         }

//         worker_out = std::thread([this]() {
//             pcap_loop(traffic_out, -1, handler_out, reinterpret_cast<u_char *>(this));
//         });
//     }

//     // traffic in
//     incoming_network_queue.add_listener(&incoming_traffic);
//     {
//         char errbuf[PCAP_ERRBUF_SIZE];
//         traffic_in = pcap_open_live(iface.c_str(), BUFSIZ, 1, 10, errbuf);
//         if (traffic_in == nullptr) {
//             std::cerr << "can't open interface: " << iface << ", err: " << errbuf << endl;
//             throw;
//         }

//         const char *filter_exp = "inbound";

//         struct bpf_program fp;
//         if (pcap_compile(traffic_in, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
//             fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
//             return;
//         }

//         if (pcap_setfilter(traffic_in, &fp) == PCAP_ERROR) {
//             fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
//             return;
//         }

//         worker_in = std::thread([this]() {
//             pcap_loop(traffic_in, -1, handler_in, reinterpret_cast<u_char *>(this));
//         });
//     }

// //    std::this_thread::sleep_for(2000ms); // ensure the pcap_loop has started

//     // sending buff out
//     fd = socket(
//             AF_PACKET,
//             SOCK_RAW,
//             htons(ETH_P_ALL)
//     );
//     if (fd == OPEN_ERROR) {
//         std::cerr << "data_link_layer_fd err" << endl;
//         printf("errno: %d\n", errno);
//         throw;
//     }

//     // get if_req
//     struct ifreq if_idx;
//     memset(&if_idx, 0, sizeof(struct ifreq));
//     strncpy(if_idx.ifr_name, iface.c_str(), IFNAMSIZ - 1);

//     if (ioctl(fd, SIOCGIFINDEX, &if_idx) == IOCTL_ERROR)
//         perror("SIOCGIFINDEX");

//     struct sockaddr_ll outputServer;
//     memset(&outputServer, 0, sizeof(struct sockaddr_ll));

//     outputServer.sll_family = AF_PACKET;
//     outputServer.sll_protocol = htons(ETH_P_ALL);
//     outputServer.sll_ifindex = if_idx.ifr_ifindex;
//     outputServer.sll_halen = ETH_ALEN;

//     if (bind(fd, (struct sockaddr *) &outputServer, sizeof(outputServer)) == BIND_ERROR)
//         perror("bind");

//     // set ll socket plain_data
// //    dest_addr.sll_ifindex = if_idx.ifr_ifindex;
// }

// iface_access_point::~iface_access_point() {
//     outgoing_network_queue.remove_listener(&this->outgoing_traffic);
//     worker_out.detach();
//     incoming_network_queue.remove_listener(&this->incoming_traffic);
//     worker_in.detach();
// }

// int iface_access_point::send_out(void* ptr, int cnt) {
// //    int res = sendto(fd,
// //                plain_data.buff, plain_data.count,
// //                0,
// //                reinterpret_cast<const sockaddr *>(&dest_addr), sizeof(dest_addr));
//     int res = send(fd,
//                ptr, cnt,
//                0);
//     if (res == SEND_ERROR || res == 0)f
//         return SEND_MEDIUM_ERROR;
//     else
//         return res;
// }




linux_iface::linux_iface(string const& iface) : iface_name(iface), async_recv(make_shared<async_recv_listener<vector<uint8_t>>>()) {

    // traffic out
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        traffic_out = pcap_open_live(iface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic_out == nullptr) {
            std::cerr << "can't open interface: " << iface << ", err: " << errbuf << endl;
            throw;
        }

        const char *filter_exp = "outbound";

        struct bpf_program fp;
        if (pcap_compile(traffic_out, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
            return;
        }

        if (pcap_setfilter(traffic_out, &fp) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_out));
            return;
        }

        worker_traffic_out = std::thread([this]() {
            pcap_loop(traffic_out, -1, handler_out, reinterpret_cast<u_char *>(this));
        });
    }

    // traffic in
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        traffic_in = pcap_open_live(iface.c_str(), BUFSIZ, 1, 10, errbuf);
        if (traffic_in == nullptr) {
            std::cerr << "can't open interface: " << iface << ", err: " << errbuf << endl;
            throw;
        }

        const char *filter_exp = "inbound";

        struct bpf_program fp;
        if (pcap_compile(traffic_in, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't parse filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
            return;
        }

        if (pcap_setfilter(traffic_in, &fp) == PCAP_ERROR) {
            fprintf(stderr, "Couldn't install filter_send %s: %s\n", filter_exp, pcap_geterr(traffic_in));
            return;
        }

        worker_traffic_in = std::thread([this]() {
            pcap_loop(traffic_in, -1, handler_in, reinterpret_cast<u_char *>(this));
        });
    }

    std::this_thread::sleep_for(2000ms); // ensure the pcap_loops have started // todo fix this

    // setup the send access
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

    struct my_send : public basic_send_medium<vector<uint8_t>> {
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
    worker_traffic_out.detach();
    worker_traffic_in.detach();
}

