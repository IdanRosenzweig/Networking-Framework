//#ifndef SERVERCLIENT_SCP_SERVER_H
//#define SERVERCLIENT_SCP_SERVER_H
//
//#include "../udp_client_server/udp_server.h"
//#include "../../protocols/scp/scp_session.h"
//#include "../../abstract/session/session_generator.h"
//
//class scp_server : public session_generator<scp_session<logical_internet_socket>> {
//public:
//    udp_server udp_server;
//
//    class sessions_handler : public basic_receiver<udp_packet_stack> {
//        scp_server *master;
//
//    public:
//        explicit sessions_handler(scp_server *master) : master(master) {
//            master->udp_server.add_listener(this);
//        }
//
//        set<ip4_addr> active_clients;
//        void handle_received_event(udp_packet_stack &&event) override {
//            if (active_clients.count(event.source_addr)) return;
//
//            session_t<scp_session<logical_internet_socket>> new_sess{1, std::make_unique<scp_session<logical_internet_socket>>()};
//            new_sess.session->gateway = &master->udp_server;
//            if (event.session->sessionData.dest_port == master->server_port)
//                master->generate_event(std::move(event));
//        }
//    };
//    sessions_handler sessionsHandler;
//
//    scp_server(int serverPort, ip4_addr src_ip, gateway* gw);
//
//};
//
//
//#endif //SERVERCLIENT_SCP_SERVER_H
