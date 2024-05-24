//#ifndef SERVERCLIENT_SCP_CLIENT_H
//#define SERVERCLIENT_SCP_CLIENT_H
//
//#include "../../abstract/connection/connection.h"
//#include "../udp_client_server/udp_client.h"
//#include "../../protocols/scp/scp_session.h"
//#include "../../protocols/socket/socket_msg.h"
//
//class scp_client : public connection {
//    udp_client udp_client;
//    struct none {};
//    scp_session<none> scp_prot;
//
//    scp_client(ip4_addr dest_ip, int dest_port, int my_port, ip4_addr src_ip, gateway * gw);
//
//    int send_data(send_msg<>&& msg) override;
//
//};
//
//
//#endif //SERVERCLIENT_SCP_CLIENT_H
