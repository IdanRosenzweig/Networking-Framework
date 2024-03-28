//#ifndef SERVERCLIENT_IP4_CONN_SERVER_H
//#define SERVERCLIENT_IP4_CONN_SERVER_H
//
//#include <unistd.h>
//#include <netinet/in.h>
//#include <cstring>
//#include "../../abstract/connectionless/basic_cl_server.h"
//#include "../linux_cl_conn.h"
//#include <memory>
//
//#include <arpa/inet.h>
//
//class ip4_conn_server : public basic_cl_server<linux_cl_conn> {
//protected:
//    std::unique_ptr<linux_cl_conn> discover_next_host() override;
//
//    int fd;
//
//public:
//    ip4_conn_server();
//};
//
//#endif //SERVERCLIENT_IP4_CONN_SERVER_H
