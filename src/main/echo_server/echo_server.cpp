#include <iostream>

#include <memory>
#include "../../temp_prot_stacks/tcp_client_server/tcp_server.h"
#include "../../temp_prot_stacks/tcp_client_server/tcp_boundary_preserving_server.h"
#include "../../abstract/session/basic_sessions_manager.h"
#include "../../abstract/session/basic_session_generator.h"

#define MY_IP "10.100.102.18"
#define HOSTING_PORT 5678

class app_handler : public basic_session_handler<tcp_boundary_preserving_session>, public msg_receiver {
public:
    void session_started() override { // when session starts, just add this class to its listeners. this calls just receives the messages and echos them
        session->add_listener(this);
    }

    explicit app_handler(unique_ptr<tcp_boundary_preserving_session> &session) : basic_session_handler(session) {}

private:
    void handle_received_event(received_msg &event) override {
        cout << "msg: " << event.data.data() + event.curr_offset << endl;

        int cnt = event.data.size() - event.curr_offset;
        send_msg send;
        memcpy(send.get_active_buff(), event.data.data() + event.curr_offset, cnt);
        send.set_count(cnt);
        session->send_data(send);
    }
};

class sess_manager : public basic_sessions_manager<tcp_boundary_preserving_session, app_handler> {
public:
    explicit sess_manager(basic_session_generator<tcp_boundary_preserving_session> *sessionsGenerator) : basic_sessions_manager<tcp_boundary_preserving_session, app_handler>(
            sessionsGenerator) {}
};

int tcp_main() {
//    tcp_server raw_tcp_server(HOSTING_PORT);
    tcp_boundary_preserving_server server(HOSTING_PORT);

    sess_manager app(&server);

    while (true) {

    }

    return 0;
}

int main() {
    tcp_main();
}