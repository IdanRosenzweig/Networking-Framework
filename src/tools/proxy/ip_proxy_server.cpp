#include "ip_proxy_server.h"

#include <iostream>
using namespace std;

ip_proxy_server::ip_proxy_server(ip4_addr src_ip, gateway* gw) : network_layer_gw(gw), server_ip(src_ip) {
    network_layer_gw->add_listener(&network_handler);

    network_handler.server = this;
}

ip_proxy_server::~ip_proxy_server() {
    network_layer_gw->remove_listener(&network_handler);
}

int conn_side_handler::send_data(send_msg_t&&data) {
    return my_conn->send_data(std::move(data));
}

void conn_side_handler::handle_callback(recv_msg_t&& data) {
    uint8_t *buff = data.buff_at_curr_offset();
    size_t cnt = data.buff_cnt_at_curr_offset();

    if (cnt <= 0) return;

    struct ip4_header header;
    extract_from_network_order(&header, buff);

    // store source addr
    my_source = header.source_addr;

    // add dest addr to mappings
    server->mappings.insert(header.dest_addr);

    // change source ip address and write header back
    header.source_addr = server->server_ip;
    header.checksum = 0; // reset checksum
    write_in_network_order(buff, &header);

    // recalculate the checksum
    ((struct ip4_header*) buff)->checksum = internet_checksum(reinterpret_cast<const uint16_t *>(buff), sizeof(ip4_header));


    // forward packet
    std::cout << "received " << cnt << " proxied bytes. sending to to ip network" << endl;
    send_msg_t send;
    memcpy(send.get_active_buff(), buff, cnt);
    send.set_count(cnt);
    server->network_handler.send_data(std::move(send));
}

int network_side_handler::send_data(send_msg_t&&data) {
    return server->network_layer_gw->send_data(std::move(data));
}

void network_side_handler::handle_callback(recv_msg_t&& data) {
    // make sure there are connections to send the packet to
    // so no nullptr dereference or shit like that happens
    if (server->conn_handler == nullptr) return;
    if (server->conn_handler->my_source == empty_ip4_addr) return;

    uint8_t *buff = data.buff_at_curr_offset();
    size_t cnt = data.buff_cnt_at_curr_offset();
    if (cnt <= 0) return;

    struct ip4_header header;
    extract_from_network_order(&header, buff);

    // change dest ip address and write header back
    header.dest_addr = server->conn_handler->my_source;
    header.checksum = 0; // reset checksum
    write_in_network_order(buff, &header);

    // recalcualte the checksum
    ((struct ip4_header*) buff)->checksum = internet_checksum(reinterpret_cast<const uint16_t *>(buff), sizeof(ip4_header));


    // forward packet
    std::cout << "sending reply back to " << convert_to_str(server->conn_handler->my_source) << ", " << cnt << " bytes" << endl;
    send_msg_t send;
    memcpy(send.get_active_buff(), buff, cnt);
    send.set_count(cnt);
    server->conn_handler->send_data(std::move(send));

}


