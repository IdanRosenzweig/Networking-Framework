#include "icmp_conn_client.h"
#include <iostream>
#include <linux/icmp.h>
#include <thread>
#include <netinet/ip.h>
#include <netdb.h>

using namespace std;

icmp_conn_client::icmp_conn_client() {

}

void icmp_conn_client::init() {
    // this whole function count have been simply ip4.init(ip) if we used it...

    fd = socket(AF_INET,
                SOCK_RAW, IPPROTO_ICMP);
    if (fd == -1) {
        cerr << "can't open socket" << endl;
        return;
    }

}

void icmp_conn_client::finish() {
    close(fd);
}


struct icmp_packet {
    icmphdr header{}; // the icmp v6 header struct for echo is the same
    char msg[56]{};

    explicit icmp_packet(const char *msg) {
        header.type = ICMP_ECHO;
        header.code = 0;
        header.un.echo.id = getpid();
        header.un.echo.sequence = 0;
        strncpy(this->msg, msg, 55);
        checksum();
    }

    uint16_t checksum() {
        this->header.checksum = 0;
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
        return this->header.checksum = ~sum;
    }
};


void icmp_conn_client::ping(const std::string &str) {
//    struct sockaddr_storage addr;
//    char* ip = dns_lookup(name.c_str(), addr);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET,
                  str.c_str(),
                  (void *) &addr.sin_addr
    ) == -1) {
        cerr << "inet_pton error" << endl;
        return;
    }

    icmp_packet packet("echo requests");
    sockaddr_storage r_addr{};
    socklen_t r_len = sizeof(r_addr);

    while (packet.header.un.echo.sequence++ < 10) { // sending 10 times once
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
        const auto start = std::chrono::high_resolution_clock::now();

        packet.checksum();
        if (sendto(fd, &packet, sizeof(packet), 0,
                   reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 1) {
            std::cerr << "Failed to send packet" << std::endl;
            continue;
        }

        icmp_packet *buffer = nullptr;
        bool failed = false;
        // only care about the reply messages from our ping
        while (buffer == nullptr || buffer->header.type != (ICMP_ECHOREPLY) ||
               buffer->header.un.echo.id != packet.header.un.echo.id) {

#define BUFF_LEN 512
            char buf[BUFF_LEN];
            memset(buf, '\x00', BUFF_LEN);
            if (recvfrom(fd, buf, BUFF_LEN, 0, reinterpret_cast<sockaddr *>(&r_addr), &r_len) < 0) {
                std::cerr << "Cannot receive from socket" << std::endl;
                failed = true;
                break;
            }

            auto ip = reinterpret_cast<iphdr *>(buf);
            buffer = reinterpret_cast<icmp_packet *>(buf + (false ? 0 : (ip->ihl << 2u)));


            switch (buffer->header.type) {
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
        }
        if (failed) continue;

        const auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> rtt = end - start;
        std::cout << "Received reply: seq=" << packet.header.un.echo.sequence
                  << " rrt=" << rtt.count() << "ms" << std::endl;
    }
}