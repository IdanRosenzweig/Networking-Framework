#ifndef SERVERCLIENT_CONN_AGGREGATOR_H
#define SERVERCLIENT_CONN_AGGREGATOR_H

#include "../sending/msg/send_msg.h"
#include "../receiving/msg/received_msg.h"
#include "connection.h"

#include <vector>
#include <memory>
class conn_aggregator;

class conn_handler;

// take the data received from a connection and send it to all the other connections in the aggregator
class conn_aggregator {
    int next_id = 0;
public:
    std::set<std::unique_ptr<conn_handler>> conns;
    void add_connection(connection* conn);
};


class conn_handler : public msg_send_medium, public msg_recv_listener {
    conn_aggregator* master;
    connection* my_conn;
    int internal_id;

public:
    conn_handler(conn_aggregator *master, connection *conn, int id);

    ~conn_handler();

    int send_data(send_msg<>&& val) override;

    void handle_received_event(received_msg &&event) override;

};


#endif //SERVERCLIENT_CONN_AGGREGATOR_H
