#include <iostream>

#include "abstract/connection_oriented/server/basic_server.h"
#include "abstract/connection_oriented/server/basic_client_handler.h"
#include "abstract/connection_oriented/client/basic_co_client.h"

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

    udp_conn_server temp_udp(4444);

    ip4_conn_server ip_server;
    ip_server.register_handler(IPPROTO_UDP, &temp_udp);

    ip_server.handle_prot_next_msg(IPPROTO_UDP);
    ip_server.handle_prot_next_msg(IPPROTO_UDP);
    ip_server.handle_prot_next_msg(IPPROTO_UDP);


    while (true) {

    }

    return 0;
}

int udp_main2() {
    std::cout << "Hello, World!" << std::endl;

    udp_conn_server server(4444);
    server.setup();

    sockaddr_in client = server.discover_next_client();
    char buff[6];
    memset(buff, '\x00', 6);

//    server.recv_data(server.clients_q.front(), buff, 5);
//    server.clients_q.front()->recv_data(buff, 5);
    server.recv_encapsulated_data(buff, 5, client);
    std::cout << "msg: " << buff << std::endl;

//    server.send_data(server.clients_q.front(), (void *) "mouse", 5);
//    server.clients_q.front()->send_data((void *) "mouse", 5);
    server.send_encapsulated_data((void *) "mouse", 5, client);

    client = server.discover_next_client();
    char buff2[6];
    memset(buff2, '\x00', 6);

//    server.recv_data(server.clients_q.front(), buff2, 5);
//    server.clients_q.front()->recv_data(buff2, 5);
    server.recv_encapsulated_data(buff2, 5, client);
    std::cout << "msg: " << buff2 << std::endl;

//    server.send_data(server.clients_q.front(), (void *) "toast", 5);
//    server.clients_q.front()->send_data((void *) "toast", 5);
    server.send_encapsulated_data((void *) "toast", 5, client);

//    server.clients_q.pop();


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
    udp_main();
//    tcp_main();
}