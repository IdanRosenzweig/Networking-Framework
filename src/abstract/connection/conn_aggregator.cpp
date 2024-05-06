#include "conn_aggregator.h"
#include <thread>

void conn_aggregator::add_connection(basic_connection *conn) {
    conns.push_back(std::make_unique<conn_handler>(this, conn));
}


conn_handler::conn_handler(conn_aggregator *master, basic_connection *conn) : master(master), my_conn(conn) {
    my_conn->add_listener(this);
}

int conn_handler::send_data(send_msg val) {
    return my_conn->send_data(val);
}

void conn_handler::handle_received_event(received_msg &event) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms);

    int i = 0;
    while (i < master->conns.size()) {
        if (master->conns[i].get() == this) {
            i++;
            continue;
        }

        int res = master->conns[i]->send_data({event.data.get() + event.curr_offset, event.sz - event.curr_offset});
        if (res == -1) {
            cerr << "couldn't send to aggregator my_conn" << endl;
            master->conns.erase(master->conns.begin() + i);
            i++;
            continue;
        }

        if (i == 0) cout << "aggregator sending to index 0" << endl;
        else if (i == 1) cout << "aggregator sending to index 1" << endl;
        i++;
    }

}
