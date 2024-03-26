#include <iostream>

#include "abstract/connection_oriented/server/basic_server.h"
#include "abstract/connection_oriented/server/basic_client_handler.h"
#include "abstract/connection_oriented/client/basic_client.h"
#include "abstract/connection_oriented/client/basic_server_handler.h"

#include "abstract/connectionless/basic_cl_server.h"
#include "abstract/connectionless/basic_cl_client.h"

#include "linux/tcp/linux_server_tcp.h"
#include "linux/udp/linux_server_udp.h"
#include "linux/ip4/linux_server_ip4.h"

#define PORT 2222

//int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    linux_server_ip4 server;
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

int main() {
    std::cout << "Hello, World!" << std::endl;

    linux_server_udp server(4444);
    server.setup();

    server.discover_next_client();
    char buff[6];
    memset(buff, '\x00', 6);

//    server.recv_data(server.clients_q.front(), buff, 5);
//    server.clients_q.front()->recv_data(buff, 5);
    server.recv_encapsulated_data(buff, 5, server.clients_q.front());
    std::cout << "msg: " << buff << std::endl;

//    server.send_data(server.clients_q.front(), (void *) "mouse", 5);
//    server.clients_q.front()->send_data((void *) "mouse", 5);
    server.send_encapsulated_data((void *) "mouse", 5, server.clients_q.front());

    char buff2[6];
    memset(buff2, '\x00', 6);

//    server.recv_data(server.clients_q.front(), buff2, 5);
//    server.clients_q.front()->recv_data(buff2, 5);
    server.recv_encapsulated_data(buff2, 5, server.clients_q.front());
    std::cout << "msg: " << buff2 << std::endl;

//    server.send_data(server.clients_q.front(), (void *) "toast", 5);
//    server.clients_q.front()->send_data((void *) "toast", 5);
    server.send_encapsulated_data((void *) "toast", 5, server.clients_q.front());

    server.clients_q.pop();


    while (true) {

    }

    return 0;
}