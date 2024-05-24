//#include "scp_client.h"
//
//scp_client::scp_client(ip4_addr dest_ip, int dest_port, int my_port, ip4_addr src_ip, gateway *gw) : udp_client(dest_ip, dest_port, my_port, src_ip, gw) {
//    // setup send flow
//    scp_prot.gateway = &udp_client;
//
//    // setup recv flow
//    udp_client.add_listener(&scp_prot);
//    scp_prot.add_listener(this);
//
//    // init session
//    scp_prot.init_session();
//}
//
//int scp_client::send_data(send_msg<> &&msg) {
//    return scp_prot.send_data(std::move(msg));
//}