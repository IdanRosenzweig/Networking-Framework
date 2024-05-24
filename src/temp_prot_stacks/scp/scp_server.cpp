//#include "scp_server.h"
//
//
//scp_server::scp_server(int serverPort, ip4_addr src_ip, gateway* gw) : udp_server(serverPort, src_ip, gw),
//                                                                       sessionsHandler(this) {
//
//
//    network_layer_gw->add_listener(&ip_server);
//
//    ip_server.protocol_handlers.assign_to_key(IPPROTO_UDP, &_udp_server);
//
//    _udp_server.port_handlers.assign_to_key(server_port, (basic_receiver<received_msg> *) this);
//}
//
//void scp_server::handle_received_event(received_msg &&event) {
//    udp_packet_stack pack_stack;
//    pack_stack.msg = std::move(event);
//    pack_stack.source_port =
//            ntohs( ((struct udp_header*) (event.data.data() + event.protocol_offsets.rbegin()->first))->source_port );
//    pack_stack.dest_port =
//            ntohs( ((struct udp_header*) (event.data.data() + event.protocol_offsets.rbegin()->first))->dest_port );
//    extract_from_network_order(&pack_stack.source_addr,
//                               (uint8_t*) &((struct iphdr*) (event.data.data() + (event.protocol_offsets[event.protocol_offsets.size() - 2]).first))->saddr);
//
//    this->receive_multiplexer<udp_packet_stack>::handle_received_event(std::move(pack_stack));
//}
//
//int scp_server::send_data_to_client(ip4_addr dest_addr, int dest_port, send_msg<> msg) {
//    ip_server.next_dest_addr.set_next_choice(dest_addr);
//    _udp_server.next_dest_port.set_next_choice(dest_port);
//
//    return _udp_server.send_data(std::move(msg));
//}
