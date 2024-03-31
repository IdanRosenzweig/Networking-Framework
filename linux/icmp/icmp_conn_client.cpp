#include "icmp_conn_client.h"
#include <iostream>
#include <linux/icmp.h>
#include <thread>
#include <netinet/ip.h>
#include <netdb.h>

using namespace std;

void icmp_conn_client::init() {
//    fd = socket(AF_INET,
//                SOCK_RAW, IPPROTO_ICMP);
//    if (fd == -1) {
//        cerr << "can't open socket" << endl;
//        return;
//    }

//    ip_client.init();
}

void icmp_conn_client::finish() {
//    close(fd);

//    ip_client.finish();
}

struct icmp_packet {
//    icmphdr header{}; // the icmp v6 header struct for echo is the same
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        uint32_t raw;
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo; // content for type echo
        struct {
            uint32_t gateway; // content for type redirect
        } redirect;
        struct {
            uint16_t __unused;
            uint16_t mtu;
        } dest_unreach; // content for type destination unreachable
    } content; // contenting varying for different types

    char msg[56]{};

    void setup_echo() {
        type = ICMP_ECHO;
        code = 0;
        content.echo.id = getpid();
        content.echo.sequence = 0;
    }

    uint16_t calc_checksum() {
        this->checksum = 0;
        int len = sizeof(*this);
        auto *current = reinterpret_cast<const uint16_t *>(this);
        uint32_t sum = 0;
        while (len > 1) {
            len -= 2;
            sum += *current++;
        }
        if (len)
            sum += *reinterpret_cast<const uint8_t *>(current);
        sum = (sum >> 16u) + (sum & 0xFFFFu);
        sum += sum >> 16u;
        return this->checksum = ~sum;
    }
};


void icmp_conn_client::ping() {
//    struct sockaddr_in addr;
//    addr.sin_family = AF_INET;
//    if (inet_pton(AF_INET,
//                  str.c_str(),
//                  (void *) &addr.sin_addr
//    ) == -1) {
//        cerr << "inet_pton error" << endl;
//        return;
//    }

    icmp_packet packet;
    packet.setup_echo();
    strcpy(packet.msg, "echo packet");

    do {

        packet.calc_checksum(); // update checksum

        // count start time
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
        const auto start = std::chrono::high_resolution_clock::now();

//        if (sendto(fd, &packet, sizeof(packet), 0,
//                   reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 1) {
//            std::cerr << "Failed to send packet" << std::endl;
//            continue;
//        }
        if (ip_client->send_next_prot_msg(IPPROTO_ICMP, &packet, sizeof(packet)) < 1) {
            std::cerr << "Failed to send packet" << std::endl;
            continue;
        }


#define BUFF_LEN 512
        char buf[BUFF_LEN];
        memset(buf, '\x00', BUFF_LEN);

//        sockaddr_storage r_addr{};
//        socklen_t r_len = sizeof(r_addr);
//        if (recvfrom(fd, buf, BUFF_LEN, 0, reinterpret_cast<sockaddr *>(&r_addr), &r_len) < 0) {
//            std::cerr << "Cannot receive from socket" << std::endl;
//            break;
//        }
        if (ip_client->recv_prot_next_msg(IPPROTO_ICMP, buf, BUFF_LEN) < 0) {
            std::cerr << "Cannot receive from socket" << std::endl;
            break;
        }
        const auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> rtt = end - start;

//        iphdr *ip = reinterpret_cast<iphdr *>(buf);
//        icmp_packet *reply = reinterpret_cast<icmp_packet *>(buf + (false ? 0 : (ip->ihl << 2u)));
        icmp_packet *reply = reinterpret_cast<icmp_packet *>(buf);

        bool failed = false;
        if (reply->content.echo.id != packet.content.echo.id) failed = true;
        switch (reply->type) {
            case ICMP_TIME_EXCEEDED: {
                std::cout << "Time Exceeded" << std::endl;
                failed = true;
                break;
            }
            case ICMP_DEST_UNREACH: {
                std::cout << "Destination Unreachable" << std::endl;
                failed = true;
                break;
            }
        }

        if (failed) {
            cout << "reply err" << endl;
        } else {
            std::cout << "Received reply: seq=" << packet.content.echo.sequence
                      << " rrt=" << rtt.count() << "ms" << std::endl;
        }

    } while (++packet.content.echo.sequence < 10);

}