#include "ip_proxy_server.h"
#include <iostream>
using namespace std;

int conn_side_handler::send_data(send_msg msg) {
    return server->connection->send_data(msg);
}

void conn_side_handler::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    cout << "received proxied packet, size " << cnt << endl;
    if (cnt <= 0) return;

    // store source ip
    ip4_addr source;
    extract_from_network_order(&source, (uint8_t*) &((struct iphdr *) buff)->saddr);
    server->source = source;

    // store dest ip in mapping
    ip4_addr dest_addr;
    extract_from_network_order(&dest_addr, (uint8_t*) &((struct iphdr *) buff)->daddr);
    server->mappings.insert(dest_addr);

    // change source ip address
    ip4_addr new_source = server->my_ip;
    write_in_network_order((uint8_t*) &((struct iphdr *) buff)->saddr, &new_source);

    cout << "sending raw bytes to ip level" << endl;
    server->network_handler.send_data({buff, cnt});
}

int network_side_handler::send_data(send_msg msg) {
    return server->network_layer_gateway->send_data(msg);
}

void network_side_handler::handle_received_event(received_msg& msg) {
    uint8_t *buff = msg.data.get() + msg.curr_offset;
    int cnt = msg.sz - msg.curr_offset;

    if (cnt <= 0) return;

    // ensure ip is in mapping
    ip4_addr dest_addr;
    extract_from_network_order(&dest_addr, (uint8_t*) &((struct iphdr *) buff)->saddr);
    if (!server->mappings.count(dest_addr)) return;

    cout << "sending reply back to tcpSession" << endl;

    // change dest ip address
    write_in_network_order((uint8_t*) &((struct iphdr *) buff)->daddr, &server->source);

    server->client_handler.send_data({buff, cnt});
}