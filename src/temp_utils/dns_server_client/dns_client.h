#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/receiving/msg/received_msg.h"
#include "../../abstract/sending/msg/msg_sender.h"
#include "../../abstract/gateway/msg_gateway.h"

#include "../../protocols/ip4/ip4_protocol.h"
#include "../../protocols/udp/udp_protocol.h"
#include "../../protocols/emp/emp.h"

#include "../../temp_prot_stacks/udp_client_server/udp_client.h"
#include "../../temp_prot_stacks/emp/emp_client.h"
#include "dns.h"

class dns_client : public recv_queue<received_msg> {
public:

    udp_client udpClient;
//    emp_client empClient;

    dns_client(ip4_addr dest_server_ip, ip4_addr src_ip, msg_gateway* network_layer_gw);

    void query(dns_record_type type, const std::string& key);

};


#endif //SERVERCLIENT_DNS_CLIENT_H
