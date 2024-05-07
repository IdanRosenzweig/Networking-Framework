#include <iostream>

#include <memory>
#include "../../temp_connections/tcp_client_server/tcp_server.h"
#include "../../temp_connections/tcp_client_server/tcp_boundary_preserving_server.h"

#define MY_IP "10.100.102.31"
#define HOSTING_PORT 5678

class server_app_tcp : public basic_receiver<std::unique_ptr<basic_session>> {

    class session_handler : public msg_receiver {
        unique_ptr<basic_session> session;

        void handle_received_event(received_msg &event) override {
            cout << "msg: " << event.data.get() + event.curr_offset << endl;
            session->send_data({event.data.get() + event.curr_offset, event.sz - event.curr_offset});
        }

    public:
        explicit session_handler(unique_ptr<basic_session> &sess) : session(std::move(sess)) {
            session->add_listener(this);
        }

    };
    vector<session_handler> sessions;

    void handle_received_event(unique_ptr<basic_session> &event) override {
        sessions.emplace_back(event);
    }

};

int tcp_main() {
//    tcp_server server(HOSTING_PORT);
    tcp_boundary_preserving_server server(HOSTING_PORT);

    server_app_tcp app;
    server.add_listener(&app);

    while (true) {

    }

    return 0;
}

int main() {
    tcp_main();
}