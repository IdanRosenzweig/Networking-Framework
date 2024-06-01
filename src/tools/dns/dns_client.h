#ifndef NETWORKING_DNS_CLIENT_H
#define NETWORKING_DNS_CLIENT_H

#include "../../abstract/receiving/recv_queue.h"
#include "../../abstract/receiving/msg/recv_msg_t.h"

#include "../../direct_connections/udp/udp_client.h"
#include "../../direct_connections/emp/emp_client.h"

#include "dns.h"

class dns_client {
public:

    udp_client udpClient;
//    emp_client empClient;
    recv_queue<udp_packet_stack> responds_recv_q;

    dns_client(ip4_addr dest_server_ip, ip4_addr src_ip, gateway* network_layer_gw);

    void query(dns_record_type type, const std::string& key);

};


#endif //NETWORKING_DNS_CLIENT_H
