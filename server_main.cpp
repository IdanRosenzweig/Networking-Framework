#include <iostream>
#include <linux/if_ether.h>

#include "abstract/connection_oriented/basic_server.h"
#include "abstract/connection_oriented/basic_co_client.h"

#include "linux/tcp/tcp_conn_server.h"
#include "protocols/udp/udp_conn_server.h"
#include "protocols/ip4/ip4_conn_server.h"
#include "linux/data_link_layer/data_link_layer_gateway.h"
//#include "linux/ip4/ip4_conn_server.h"
#include "protocols/ether/ethernet_conn_server.h"
#include "protocols/ether/ethernet_conn_client.h"
#include "linux/network_layer_gateway/network_layer_gateway_client.h"
#include "abstract/basic_recv_conn.h"
#include "linux/network_layer_gateway/network_layer_gateway_server.h"
#include <netinet/ip.h>
#include <set>

#define PORT 4444

class udp_app : public basic_recv_conn {
public:
    basic_send_conn *gateway;

    void print() {
        msg_protocol_stack msg = std::move(recv_data());
        cout << "client: " << (char *) (msg.msg.data.get() + msg.curr_offset) << endl;
    }

    void send(string str) {
        gateway->send_data((void *) str.c_str(), str.size());
    }
};

int udp_main() {
    udp_app udpApp;

    // protocol stack
    network_layer_gateway_server networkLayerGateway;

    ip4_conn_server ip_server;
    udp_conn_server udp_client;

    // setup send flow
    ip_server.gateway = &networkLayerGateway;
    ip_server.next_external_protocol.set_next_choice(IPPROTO_UDP);
    ip_server.next_client_addr.set_next_choice(convert_to_ip4_addr("10.100.102.18"));

    udp_client.gateway = &ip_server;
    udp_client.server_port.set_next_choice(PORT);
    udp_client.next_external_port.set_next_choice(1212);

    // setup recv flow
    networkLayerGateway.add_listener(&ip_server);

    ip_server.prot_handlers[IPPROTO_UDP] = &udp_client;

    // setup app
    udpApp.gateway = &udp_client; // send
    udp_client.port_handlers[PORT] = &udpApp; // recv

    // communicate

    while (true) {
        udpApp.print();
        udpApp.send("servr");
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

class to_client_sys : public basic_send_conn, public basic_recv_conn {
public:
    basic_send_conn *gateway;

    int send_data(void *buff, int count) override {
        return gateway->send_data(buff, count);
    }
};

class from_internet_sys : public basic_recv_conn {
public:
};

void proxy_main() {
    // proxy at the ip level

    // connection to the client using udp
    network_layer_gateway_server networkLayerGateway;

    ip4_conn_server ip_server;
    udp_conn_server udp_client;

    // setup send flow
    ip_server.gateway = &networkLayerGateway;
    ip_server.next_external_protocol.set_next_choice(IPPROTO_UDP);
    ip_server.next_client_addr.set_next_choice(convert_to_ip4_addr("10.100.102.18"));

    udp_client.gateway = &ip_server;
    udp_client.server_port.set_next_choice(PORT);
    udp_client.next_external_port.set_next_choice(1212);

    // setup recv flow
    networkLayerGateway.add_listener(&ip_server);

    ip_server.prot_handlers[IPPROTO_UDP] = &udp_client;


    // gateway to ip level
    network_layer_gateway_client gateway;

    // to_client_sys
    class to_client_sys sys;
    sys.gateway = &udp_client;
    udp_client.port_handlers[PORT] = &sys;

    // from_inernet_sys
    class from_internet_sys fromInternetSys;
    gateway.add_listener(&fromInternetSys);

    // proxy mappings
    set<ip4_addr> mapping;

    std::thread from_host([&]() {
#define BUFF_LEN 1024
        char buff[BUFF_LEN];
        while (true) {
            memset(buff, 0, BUFF_LEN);

            msg_protocol_stack msg = std::move(sys.recv_data());
            uint8_t * buff = msg.msg.data.get() + msg.curr_offset;
            int cnt = msg.msg.sz - msg.curr_offset;

            cout << "received proxied packet, size " << cnt << endl;
            if (cnt <= 0) continue;

            // change source ip address
            ((struct iphdr *) buff)->saddr = inet_addr("10.100.102.18");

            // add dest ip addr to mapping
            mapping.insert(ip4_addr{((struct iphdr *) buff)->daddr});

            cout << "sending raw bytes to ip level" << endl;
//        ether_client.send_next_msg(buff, cnt);
            gateway.send_data(buff, cnt);
        }
    });

    std::thread to_host([&]() {
#define BUFF_LEN 1024
        char buff[BUFF_LEN];
        while (true) {
            memset(buff, 0, BUFF_LEN);

            msg_protocol_stack msg = std::move(fromInternetSys.recv_data());
            uint8_t * buff = msg.msg.data.get() + msg.curr_offset;
            int cnt = msg.msg.sz - msg.curr_offset;

            if (cnt <= 0) continue;
            cout << "proxied reply, size " << cnt << endl;

            // ensure ip is in mapping
            ip4_addr addr{((struct iphdr *) buff)->saddr};
            if (!mapping.count(addr)) continue;

            cout << "sending reply back to client" << endl;

            // change dest ip address
            ((struct iphdr *) buff)->daddr = inet_addr("10.100.102.18");

            sys.send_data(buff, cnt);
        }
    });

    from_host.join();
    to_host.join();
}

int main() {
//    udp_main();
//    tcp_main();
    proxy_main();
}