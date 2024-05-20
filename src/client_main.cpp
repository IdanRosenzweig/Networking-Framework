#include <iostream>
#include <vector>
#include <linux/if_ether.h>
#include <pcap/pcap.h>
#include <thread>
#include <net/if.h>
#include <sys/ioctl.h>

#include "protocols/udp/udp_protocol.h"
#include "temp_utils/dns_server_client/dns_client.h"
#include "protocols/ip4/ip4_protocol.h"
#include "protocols/icmp/icmp_protocol.h"
#include "protocols/ether/ethernet_protocol.h"
#include "temp_utils/arp/arp_scanner.h"

#include "linux/interface_gateway.h"
#include "protocols/udp/udp_protocol.h"
#include "temp_utils/proxy/ip_proxy_client.h"
#include "abstract/utils/circular_buffer.h"
#include "abstract/receiving/recv_queue.h"
#include "temp_prot_stacks/udp_client_server/udp_client.h"
#include "protocols/tcp/tcp_protocol.h"

#include "temp_utils/firewalls/firewall.h"
#include "temp_utils/firewalls/network_layer/only_udp_filter.h"
#include "temp_utils/analyzer/net_analyzer.h"

#include "linux/virtual_if.h"

#include "temp_prot_stacks/ssh/ssh_conn_session.h"
#include "temp_utils/vpn/common.h"
#include "temp_utils/vpn/vpn_client.h"
#include "temp_prot_stacks/tcp_client_server/tcp_client.h"
#include "protocols/msg_boundary/msg_boundary_seperator.h"

#define MY_IP "10.100.102.18"

class udp_app_client : public recv_queue<received_msg> {
public:
    msg_sender *gateway;

    void print() {
        received_msg msg = front_data();
        cout << "aggregator: " << (char *) (msg.data.data() + msg.curr_offset) << endl;
    }

    void send(string str) {
        gateway->send_data({(void *) str.c_str(), (int) str.size()});
    }
};


void udp_main() {

    constexpr int my_port = 1212;
    constexpr int server_port = 4444;
    string server_addr{"10.100.102.31"};

    // network layer gateway protected by firewall
    network_layer_gateway networkLayerGateway("enp0s3");

//    firewall fw(&networkLayerGateway);
//    fw.filters.push_back(new only_udp_filter<my_port>());

    // protocol stack
    ip4_protocol ip_client;
    bs_port_multiplexing udp_client;

    udp_app_client app_system;

    // setup send flow
    ip_client.gateway = &networkLayerGateway;
    ip_client.next_protocol.set_next_choice(IPPROTO_UDP);
    ip_client.next_dest_addr.set_next_choice(convert_to_ip4_addr(server_addr));
    ip_client.next_source_addr.set_next_choice(convert_to_ip4_addr(MY_IP));

    udp_client.gateway = &ip_client;
    udp_client.next_source_point.set_next_choice(my_port);
    udp_client.next_dest_point.set_next_choice(server_port);

    app_system.gateway = &udp_client;

    // setup recv flow
    networkLayerGateway.add_listener(&ip_client);

    ip_client.protocol_handlers.assign_to_key(IPPROTO_UDP, &udp_client);

    udp_client.port_handlers.assign_to_key(my_port, &app_system);

    // communicate
    for (int i = 0; i < 5; i++) {
        app_system.send("clint");
        app_system.print();
    }

    while (true) {

    }
}

//void ssh_main() {
//    ssh_conn_session tcpSession("10.100.102.18", "idan", "123", 1234);
//
//    cout << "ssh tcpSession started" << endl;
//
////    tcpSession.execute_remote_cli_command("ls -la");
////    tcpSession.execute_remote_cli_command("id");
//    send_msg msg;
//    char buff[10] = "hello";
//    msg.buff = buff;
//    msg.count = 10;
//    cout << "sent " << tcpSession.send_data(msg) << endl;
//
//    while (true) {
//
//    }
//}

int main() {

//    udp_main();

    return (0);
}

