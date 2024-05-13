#ifndef SERVERCLIENT_DNS_SERVER_H
#define SERVERCLIENT_DNS_SERVER_H

#include <fstream>
#include <string>

#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/sending/msg_sender.h"

#include "dns.h"
#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../temp_connections/udp_client_server/udp_server.h"


class dns_server : public basic_receiver<socket_msg> {
    udp_server udpServer;

public:
    std::map<string, ip4_addr> mappings_type_a = {
//            {"google.com", convert_to_ip4_addr("172.217.22.78")}
    };
    std::map<string, string> mappings_type_mx = {
//            {"google.com", "smtp.google.com"}
    };


    dns_server() : udpServer(DNS_SERVER_PORT) {
        udpServer.add_listener(this);
    }

    void handle_received_event(socket_msg &event) override;

};


#endif //SERVERCLIENT_DNS_SERVER_H
