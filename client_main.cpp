#include <iostream>
#include <vector>

#include "linux/tcp/linux_client_tcp.h"
#include "linux/udp/linux_client_udp.h"
#include "linux/dns/linux_client_dns.h"
#include "linux/ip4/linux_client_ip4.h"

#include "abstract/basic_gateway.h"
#include "abstract/basic_encapsulating_protocol.h"

#define IP "127.0.0.1"
#define PORT 4444

//int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    linux_client_ip4 client(IP);
//    client.conn_to_server();
//
////    std::cout << "sending data" << std::endl;
////    char* msg = "hello";
////    int len = strlen(msg);
////    cout << "sent " << client.server->send_data(msg, len) << " bytes" << endl;
////    std::cout << "data sent" << std::endl;
//
//    while (true) {
//
//    }
//
//
//    return 0;
//}

//int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    linux_client_udp client(IP, PORT);
//    client.init();
//
//    std::cout << "sending data" << std::endl;
//    char* msg = "hello";
//    int len = strlen(msg);
////    cout << "sent " << client.send_data(msg, len) << " bytes" << endl;
//    cout << "sent " << client.send_encapsulated_data(msg, len) << " bytes" << endl;
//    std::cout << "data sent" << std::endl;
//
//    char buff[6];
//    memset(buff, '\x00', 6);
////    client.recv_data(buff, 5);
//    client.recv_encapsulated_data(buff, 5);
//    cout << "server: " << buff << endl;
//
//
//    std::cout << "sending data" << std::endl;
//    char* msg2 = "secnd";
//    int len2 = strlen(msg2);
////    cout << "sent " << client.send_data(msg2, len2) << " bytes" << endl;
//    cout << "sent " << client.send_encapsulated_data(msg2, len2) << " bytes" << endl;
//    std::cout << "data sent" << std::endl;
//
//    char buff2[6];
//    memset(buff2, '\x00', 6);
////    client.recv_data(buff2, 5);
//    client.recv_encapsulated_data(buff2, 5);
//    cout << "server: " << buff2 << endl;
//
//    while (true) {
//
//    }
//
//    return 0;
//}


int main() {
    vector<string> hosts = {
            "wiki.osdev.org",
            "www.wix.com",
            "docs.google.com",
            "www.scs.stanford.edu",
            "yahho.com",
            "google.com",
            "youtube.com",
            "tradingview.com",
            "walla.co.il",
            "nasa.com",
            "medium.com",
    };

    linux_client_dns client("8.8.8.8");
    client.init();
    for (string& str : hosts)
        client.query(str);

    return 0;
}