#include "conn_aggregator.h"
#include <thread>
#include <iostream>

using namespace std;

void conn_aggregator::add_connection(connection *conn) {
    conns.insert(std::make_unique<conn_handler>(this, conn, next_id++));
}

conn_handler::conn_handler(conn_aggregator *master, connection *conn, int id) : master(master), my_conn(conn), internal_id(id) {
    my_conn->add_listener(this);
}

conn_handler::~conn_handler() {
    my_conn->remove_listener(this);
}

int conn_handler::send_data(send_msg<> &&val) {
    return my_conn->send_data(std::move(val));
}

void conn_handler::handle_received_event(received_msg &&event) {
    auto it = master->conns.begin();
    while (it != master->conns.end()) {
        if ((*it)->internal_id == this->internal_id) {
            it++;
            continue;
        }

        send_msg<> send;
        int cnt = event.data.size() - event.curr_offset;
        memcpy(send.get_active_buff(), event.data.data() + event.curr_offset, cnt);
        send.set_count(cnt);

        int res = (*it)->send_data(std::move(send));
        if (res == SEND_MEDIUM_ERROR || res == 0) {
            std::cerr << "couldn't forward data to some connection, removing it from the aggregator" << endl;
            it = master->conns.erase(it);
            continue;
        }

        std::cout << "forwarded data to connection with it " << (*it)->internal_id << endl;
        it++;
    }

}
