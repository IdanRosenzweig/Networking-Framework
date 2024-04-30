#include <iostream>
#include <linux/if_ether.h>

#include "protocols/udp/udp_protocol.h"
#include "protocols/tcp/tcp_protocol.h"
#include "protocols/ip4/ip4_protocol.h"
#include "linux/data_link_layer/data_link_layer_gateway.h"
#include "protocols/ether/ethernet_protocol.h"
#include "linux/network_layer_gateway/network_layer_gateway.h"
#include "abstract/receiving/recv_queue.h"
#include "linux/network_layer_gateway/network_layer_gateway.h"
#include "proxy/network_layer/ip_proxy_server.h"
#include "temp_connections/udp/udp_server.h"
#include "temp_connections/dns_server/dns_server.h"
#include "abstract/connection/basic_forwarder_daemon.h"
#include "temp_connections/ssh/ssh_server.h"
#include "vpn/vpn_daemon.h"

#define MY_IP "10.100.102.31"

class udp_app_server : public recv_queue<received_msg> {
public:
    msg_sender *gateway;

    void print() {
        received_msg msg = front_data();
        cout << "client: " << (char *) (msg.data.get() + msg.curr_offset) << endl;
    }

    void send(string str) {
        gateway->send_data({(void *) str.c_str(), (int) str.size()});
    }
};

int udp_main() {

    constexpr int hosting_port = 4444;
    constexpr int client_port = 1212;
    string client_addr{"10.100.102.18"};

    // protocol stack
    network_layer_gateway networkLayerGateway;

    ip4_protocol ip_server;
    udp_protocol udp_client;

    udp_app_server udpApp;

    // setup send flow
    ip_server.gateway = &networkLayerGateway;
    ip_server.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_server.next_dest_addr.set_next_choice(convert_to_ip4_addr(client_addr));
    ip_server.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

    udp_client.gateway = &ip_server;
    udp_client.next_source_port.set_next_choice(hosting_port);
    udp_client.next_dest_port.set_next_choice(client_port);

    udpApp.gateway = &udp_client;

    // setup recv flow
    networkLayerGateway.add_listener(&ip_server);

    ip_server.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_client);

    udp_client.port_handlers.assign_to_key(hosting_port, &udpApp);

    // communicate
    for (int i = 0; i < 40; i++) {
        udpApp.print();
        udpApp.send("servr");
    }

    while (true) {

    }

    return 0;
}

class test_handler : public msg_receiver {
public:
    void handle_received_event(received_msg& msg) override {
        cout << "received: " << msg.data.get() + msg.curr_offset << endl;
    }
};

int tcp_main() {
    std::cout << "Hello, World!" << std::endl;

    tcp_protocol tcp_server;

    while (tcp_server.as_server_sessions.empty()) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(500ms);
    }

    // has session
    cout << "has session" << endl;
    std::unique_ptr<basic_session>& session = tcp_server.as_server_sessions[0];

    test_handler testHandler;
    session->add_listener(&testHandler);

    while (true) {

    }

    return 0;
}


class proxy_app_server : public basic_receiver<socket_msg>, public basic_connection {
public:
    udp_server* udpServer;

    proxy_app_server(udp_server* udpServer) : udpServer(udpServer) {
        udpServer->add_listener(this);
    }

    void handle_received_event(socket_msg& event) override {
//        cout << "received from " << inet_ntoa(in_addr{htonl(event.source_addr)}) << endl;
        cout << "source port " << event.source_port << endl;
        cout << "dest port " << event.dest_port << endl;
        this->listenable::handle_received_event(event.msg);
    }

    string client_addr = "10.100.102.18";
    int send_data(send_msg val) override {
        return udpServer->send_data_to_client(convert_to_ip4_addr(client_addr), 1001, val);
    }
};

void proxy_main1() {
    udp_server udpConnServer(4001);
//    icmp_connection_server icmpConnectionServer;
    proxy_app_server conn(&udpConnServer); // automatically sets up send and recv control

    network_layer_gateway ipNetworkGateway; // gateway to ip network

    ip_proxy_server proxy(&conn, &ipNetworkGateway);

    while (true) {

    }

}

void proxy_main2() {
    udp_server udpConnServer(4002); // connection to the client
//    icmp_connection_server icmpConnectionServer;
    proxy_app_server conn(&udpConnServer);

    network_layer_gateway ipNetworkGateway; // gateway to ip network

    ip_proxy_server proxy(&conn, &ipNetworkGateway);

    while (true) {

    }

}

void proxy_main3() {
    udp_server udpConnServer(4003); // connection to the client
//    icmp_connection_server icmpConnectionServer;
    proxy_app_server conn(&udpConnServer);

    network_layer_gateway ipNetworkGateway; // gateway to ip network

    ip_proxy_server proxy(&conn, &ipNetworkGateway);

    while (true) {

    }

}

void proxy_main() {
    std::thread proxy1([]() -> void {
        proxy_main1();
    });
//    std::thread proxy2([]() -> void {
//        proxy_main2();
//    });
//    std::thread proxy3([]() -> void {
//        proxy_main3();
//    });

    proxy1.join();
//    proxy2.join();
//    proxy3.join();
}

void dns_main() {
    dns_server server;

    while (true) {

    }
}


void vpn_main() {
    vpn_daemon vpnDaemon;

    while (true) {}
}

//void ssh_main() {
//    ssh_server server(1234);
//
//    while (server.as_server_sessions.empty()) {
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(500ms);
//    }
//
//    // has session
//    cout << "has session" << endl;
//    std::unique_ptr<basic_session>& session = server.as_server_sessions[0];
//
//    test_handler testHandler;
//    session->add_listener(&testHandler);
//
//    while (true) {
//
//    }
//}

int main() {
//    udp_main();
//    tcp_main();
//    proxy_main();
//    dns_main();
    vpn_main();

}