#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/gateway/msg_gateway.h"

#include "../../linux/osi/network_layer_gateway.h"
#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/better_suite/bs_emp/bs_emp.h"

#include "../../temp_prot_stacks/udp_client_server/udp_client.h"
#include "dns.h"

class dns_client : public recv_queue<received_msg> {
public:

//    msg_gateway * gateway;
//    ip4_protocol ip_client;
//    udp_protocol udp_client;
    udp_client udpClient;

//    bs_emp empClient;

    dns_client(ip4_addr, msg_gateway* gw = nullptr);

    void query(dns_record_type type, const std::string& key);

};


#endif //SERVERCLIENT_DNS_CLIENT_H
