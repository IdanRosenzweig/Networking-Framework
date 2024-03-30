#include "ip4_conn_server.h"
#include <iostream>
#include <linux/if_ether.h>
#include <netinet/ip.h>

using namespace std;

unsigned short checksum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

void parse_packet(unsigned char *packet, ssize_t packet_size) {
    struct iphdr *iph = (struct iphdr *)packet;
    unsigned char *data = packet + iph->ihl * 4; // Skip IP header

    printf("Received packet with source IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->saddr));
    printf("Payload data: %s\n", data);
}

sockaddr_in ip4_conn_server::discover_next_host() {
    /*
    // we are currently only interested in discovering the address of the client that sends the next data
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

#define LEN 200
    unsigned char buff[LEN];
    memset(buff, '\x00', LEN);
    cout << "waiting for data" << endl;
    int sz;
    if ((sz = recvfrom(fd, (char*) buff, LEN,
                 0, // this would provide us with the address of the client, without reading any data from the queue
                 (struct sockaddr *) &client_addr, &len)) == -1) {
        printf("Couldn't receive\n");
        return nullptr;
    }
    parse_packet(buff, sz);
    printf("Received message from IP: %s and port: %i\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    auto *host = new linux_cl_conn;
    host->fd = fd; // the server's open fd
    host->addr = client_addr;
    return std::unique_ptr<linux_cl_conn>(host);
*/


//    // we are currently only interested in discovering the address of the client that sends the next data
//    struct sockaddr_in client_addr;
//    socklen_t len = sizeof(client_addr);
//
//    if (recvfrom(fd, nullptr, 0, MSG_PEEK, // this would provide us with the address of the client, without reading the data from the queue
//                 (struct sockaddr*)&client_addr, &len) == -1){
//        printf("Couldn't receive\n");
//        throw;
//    }
//    printf("Received message from IP: %s and port: %i\n",
//           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
//
//    return client_addr;
}

ip4_conn_server::ip4_conn_server() {
    // todo move this to setup() and destroy()

//    // opening a file descriptor
//    int fd = socket(
//            AF_INET,
//            SOCK_RAW,
//            IPPROTO_UDP
//    );
//    if (fd == -1) {
//        cerr << "socket err" << endl;
//        printf("errno: %d\n", errno);
//        return;
//    }
//
//    int one = 1;
//    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
//        perror("setsockopt");
//        return;
//    }
//
//    // binding a socket
//    struct sockaddr_in addr;
//    bzero((void *) &addr, sizeof(addr));
//    addr.sin_family = AF_INET;
////    addr.sin_port = htons(4444);
//    addr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//    if (bind(
//            fd,
//            (struct sockaddr *) &addr,
//            sizeof(addr)
//    ) == -1) {
//        cerr << "bind err" << endl;
//        return;
//    }

//        // listening on the socket
//        if (listen(fd, 1) == -1) {
//            cerr << "listen err";
//            return;
//        }

    cout << "waiting" << endl;
}

void ip4_conn_server::register_handler(int prot, encapsulated_data_handler *handler) {
    int fd = socket(
            AF_INET,
            SOCK_RAW,
            prot
    );
    if (fd == -1) {
        cerr << "socket err" << endl;
        printf("errno: %d\n", errno);
        throw;
    }

    prot_handlers[prot] = {fd, handler};
}


void ip4_conn_server::handle_prot_next_msg(int prot) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);


#define PACKET_LEN 256
    char packet[PACKET_LEN];
    memset(&packet, '\x00', PACKET_LEN);
    int res = recvfrom(prot_handlers[prot].fd,
                       packet, PACKET_LEN,
                       0,
                       (struct sockaddr *)&client_addr, &len);

    struct iphdr* ip_hdr = reinterpret_cast<iphdr *>(packet);

    char *data = packet + sizeof(struct iphdr);

    cout << "received ip packet with protocol: " << prot << endl;

    cout << "calling handler" << endl;
    prot_handlers[prot].handler->handler_received_data(data);


}

