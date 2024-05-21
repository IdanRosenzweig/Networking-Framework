#include "conn_aggregator.h"
#include <thread>
#include <iostream>

using namespace std;

void conn_aggregator::add_connection(msg_connection *conn) {
    conns.push_back(std::make_unique<conn_handler>(this, conn));
}


conn_handler::conn_handler(conn_aggregator *master, msg_connection *conn) : master(master), my_conn(conn) {
    my_conn->add_listener(this);
}

int conn_handler::send_data(send_msg<> &val) {
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

        send_msg<> send;
        int cnt = event.data.size() - event.curr_offset;
        memcpy(send.get_active_buff(), event.data.data() + event.curr_offset, cnt);
        send.set_count(cnt);

        int res = master->conns[i]->send_data(send);
        if (res == -1) {
            cerr << "couldn't send to aggregator my_conn" << endl;
            master->conns.erase(master->conns.begin() + i);
            i++;
            continue;
        }

        if (i == 0) cout << "aggregator sending to index 1: " << cnt << ", was at offset " << event.curr_offset << endl;
        else if (i == 1)
            cout << "aggregator sending to index 1: " << cnt << ", was at offset " << event.curr_offset << endl;
        i++;
    }

}
