#ifndef SERVERCLIENT_DNS_CLIENT_H
#define SERVERCLIENT_DNS_CLIENT_H

#include "../../abstract/basic_recv_conn.h"
#include "../../abstract/basic_send_conn.h"

using namespace std;

#define DNS_PORT 53

class dns_client : public basic_recv_conn {
public:

    basic_send_conn* gateway;

    void query(const string &name);

};


#endif //SERVERCLIENT_DNS_CLIENT_H
