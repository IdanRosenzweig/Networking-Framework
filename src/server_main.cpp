#include <iostream>
#include <linux/if_ether.h>

#include "protocols/udp/udp_protocol.h"
#include "protocols/tcp/tcp_protocol.h"
#include "protocols/ip4/ip4_protocol.h"
#include "linux/interface_gateway.h"
#include "protocols/ether/ethernet_protocol.h"
#include "linux/osi/network_layer_gateway.h"
#include "abstract/receiving/recv_queue.h"
#include "linux/osi/network_layer_gateway.h"
#include "temp_utils/proxy/ip_proxy_server.h"
#include "temp_prot_stacks/udp_client_server/udp_server.h"
#include "temp_utils/dns_server_client//dns_server.h"
#include "abstract/connection/conn_aggregator.h"
#include "temp_prot_stacks/ssh/ssh_server.h"
#include "temp_utils/vpn/vpn_daemon.h"
#include "protocols/msg_boundary/msg_boundary_seperator.h"

#define MY_IP "10.100.102.31"

class server_app_udp : public recv_queue<received_msg> {
public:
    msg_sender *gateway;

    void print() {
        received_msg msg = front_data();
        cout << "tcpSession: " << (char *) (msg.data.data() + msg.curr_offset) << endl;
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
    network_layer_gateway networkLayerGateway("enp0s3");

    ip4_protocol ip_server;
    bs_port_multiplexing udp_client;

    server_app_udp udpApp;

    // setup send flow
    ip_server.gateway = &networkLayerGateway;
    ip_server.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_server.next_dest_addr.set_next_choice(convert_to_ip4_addr(client_addr));
    ip_server.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

    udp_client.gateway = &ip_server;
    udp_client.next_source_point.set_next_choice(hosting_port);
    udp_client.next_dest_point.set_next_choice(client_port);

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


//void ssh_main() {
//    ssh_server raw_tcp_server(1234);
//
//    while (raw_tcp_server.as_server_sessions.empty()) {
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(500ms);
//    }
//
//    // has session
//    cout << "has session" << endl;
//    std::unique_ptr<basic_session>& session = raw_tcp_server.as_server_sessions[0];
//
//    server_app_tcp testHandler;
//    session->add_listener(&testHandler);
//
//    while (true) {
//
//    }
//}

int main() {
//    udp_main();
//    vpn_main();

}