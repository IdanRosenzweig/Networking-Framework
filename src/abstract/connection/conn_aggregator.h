#ifndef SERVERCLIENT_CONN_AGGREGATOR_H
#define SERVERCLIENT_CONN_AGGREGATOR_H

#include "../sending/msg/msg_sender.h"
#include "../receiving/msg/msg_receiver.h"
#include "connection.h"
#include "../gateway/gateway.h"
#include <vector>
class conn_aggregator;

class conn_handler;

// aggregates multiple connections together
class conn_aggregator {
public:
    std::vector<std::unique_ptr<conn_handler>> conns;

    conn_aggregator() {}

    void add_connection(connection* conn);

};


class conn_handler : public msg_sender, public msg_receiver {
    conn_aggregator* master;
    connection* my_conn;

public:

    conn_handler(conn_aggregator *master, connection *conn);

    ~conn_handler();

    int send_data(send_msg<>&& val) override;

    void handle_received_event(received_msg &&event) override;

};


#endif //SERVERCLIENT_CONN_AGGREGATOR_H
