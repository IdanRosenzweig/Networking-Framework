#include <iostream>
#include <linux/if_ether.h>

#include "abstract/connection_oriented/basic_server.h"
#include "abstract/connection_oriented/basic_co_client.h"

#include "linux/tcp/tcp_conn_server.h"
#include "linux/udp/udp_conn_server.h"
#include "linux/ip4/ip4_conn_server.h"
#include "linux/data_link_layer/data_link_layer_gateway.h"
//#include "linux/ip4/ip4_conn_server.h"
#include "linux/ether/ethernet_conn_server.h"

#define PORT 4444

int udp_main() {
    std::cout << "Hello, World!" << std::endl;

    ethernet_conn_server ether_server;

    ip4_conn_server ip_server;
    ip_server.setNextClient(ip4_addr{inet_network("10.100.102.18")});
    ip_server.ether_server = &ether_server;

    udp_conn_server udp_client(PORT);
    udp_client.setNextPort(1212);
    udp_client.ip_server = &ip_server;

    char buff[6];
    memset(buff, '\x00', 6);
    udp_client.recv_data(buff, 5);
    cout << "msg: " << buff << endl;


    cout << "sending data" << endl;
    char* data = "servr";
    udp_client.send_data(data, 5);


    udp_client.recv_data(buff, 5);
    cout << "msg: " << buff << endl;


    cout << "sending data" << endl;
    data = "toast";
    udp_client.send_data(data, 5);


    while (true) {

    }

    return 0;
}

int tcp_main() {
    std::cout << "Hello, World!" << std::endl;

    tcp_conn_server server(PORT);
    server.setup();

    handler client = server.conn_next_host();
    char buff[6];
    memset(buff, '\x00', 6);

    client.recv_encapsulated_data(buff, 5);
    std::cout << "msg: " << buff << std::endl;

    client.send_encapsulated_data((void *) "mouse", 5);

    char buff2[6];
    memset(buff2, '\x00', 6);

    client.recv_encapsulated_data(buff2, 5);
    std::cout << "msg: " << buff2 << std::endl;

    client.send_encapsulated_data((void *) "toast", 5);


    while (true) {

    }


    return 0;
}

void tunnel_main() {
    // tunenl using udp
    ethernet_conn_server ether_server;

    ip4_conn_server ip_server;
    ip_server.ether_server = &ether_server;
    ip_server.setNextClient(ip4_addr{inet_network("10.100.102.18")});

    udp_conn_server udp_server(PORT);
    udp_server.setNextPort(1212);
    udp_server.ip_server = &ip_server;

    // gateway to forward packets
    data_link_layer_gateway gateway;

//    std::thread from_host([&]() {
//#define BUFF_LEN 1024
//        char buff[BUFF_LEN];
//        while (true) {
//            memset(buff, 0, BUFF_LEN);
//
//            int cnt = udp_server.recv_data(buff, BUFF_LEN);
//            cout << "received tunneled packet, size " << cnt << endl;
//
//            if (cnt == 0) continue;
//
//            cout << "sending raw bytes to data link layer" << endl;
//            gateway.send_raw(buff, cnt);
//
//        }
//    });

    std::thread to_host([&]() {
#define BUFF_LEN 1024
        char reply[BUFF_LEN];
        while (true) {
            memset(reply, 0, BUFF_LEN);

            int reply_len = gateway.recv_raw(reply, BUFF_LEN);
            cout << "received reply with len " << reply_len << endl;

            if (reply_len == 0) continue;

            cout << "sending raw bytes back in the tunnel" << endl;
            udp_server.send_data(reply, reply_len);
        }
    });

//    from_host.join();
    to_host.join();
}

int main() {
//    udp_main();
//    tcp_main();
    tunnel_main();
}