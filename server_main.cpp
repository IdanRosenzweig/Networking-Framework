#include <iostream>

#include "abstract/connection_oriented/basic_server.h"
#include "abstract/connection_oriented/basic_co_client.h"

#include "abstract/connectionless/basic_cl_server.h"
#include "abstract/connectionless/basic_cl_client.h"

#include "linux/tcp/tcp_conn_server.h"
#include "linux/udp/udp_conn_server.h"
#include "linux/ip4/ip4_conn_server.h"
//#include "linux/ip4/ip4_conn_server.h"

#define PORT 2222

//int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    ip4_conn_server server;
//
//    while (true) {
//        server.discover_next_client();
//
////        char buff[6];
////        memset(buff, '\x00', 6);
////        server.clients_q.front()->recv_data(buff, 5);
////        server.clients_q.pop();
//
////        std::cout << "msg: " << buff << std::endl;
//    }
//
//    return 0;
//}

int udp_main() {
    std::cout << "Hello, World!" << std::endl;

    ip4_conn_server ip_server;
    ip_server.register_handler(IPPROTO_UDP);

    udp_conn_server udp_client(4444);
    udp_client.ip_server = &ip_server;

    char buff[6];
    memset(buff, '\x00', 6);
    udp_client.recv_data(buff, 5);
    cout << "msg: " << buff << endl;

    cout << "sending data" << endl;
    char* data = "servr";
    udp_client.send_data(data, 5);


    udp_client.recv_data(buff, 5); // first one actually captures this server's response, and ignores it
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

    tcp_conn_server server(4444);
    server.setup();

    handler client = server.conn_next_host();
    char buff[6];
    memset(buff, '\x00', 6);

//    server.recv_data(server.clients_q.front(), buff, 5);
//    server.clients_q.front()->recv_data(buff, 5);
    client.recv_encapsulated_data(buff, 5);
    std::cout << "msg: " << buff << std::endl;

//    server.send_data(server.clients_q.front(), (void *) "mouse", 5);
//    server.clients_q.front()->send_data((void *) "mouse", 5);
    client.send_encapsulated_data((void *) "mouse", 5);

    char buff2[6];
    memset(buff2, '\x00', 6);

//    server.recv_data(server.clients_q.front(), buff2, 5);
//    server.clients_q.front()->recv_data(buff2, 5);
    client.recv_encapsulated_data(buff2, 5);
    std::cout << "msg: " << buff2 << std::endl;

//    server.send_data(server.clients_q.front(), (void *) "toast", 5);
//    server.clients_q.front()->send_data((void *) "toast", 5);
    client.send_encapsulated_data((void *) "toast", 5);

//    server.clients_q.pop();


    while (true) {

    }


    return 0;
}

int main() {
//    udp_main();
    tcp_main();
}