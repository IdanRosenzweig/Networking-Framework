#include "../../temp_connections/tcp_client_server/tcp_boundary_preserving_client.h"
#include "../../temp_connections/tcp_client_server/tcp_boundary_preserving_server.h"
#include "../../abstract/session/basic_sessions_manager.h"
#include <unistd.h>

class app_handler : public basic_session_handler<tcp_boundary_preserving_session>, public msg_receiver {
public:
    void
    session_started() override { // when session starts, just add this class to its listeners. this calls just receives the messages and echos them
        session->add_listener(this);
    }

    explicit app_handler(unique_ptr<tcp_boundary_preserving_session> &session) : basic_session_handler(session) {}

private:
    void handle_received_event(received_msg &event) override {
        cout << event.data.get() + event.curr_offset << endl;
//        session->send_data({event.data.get() + event.curr_offset, event.sz - event.curr_offset});
    }
};

class sess_manager : public basic_sessions_manager<tcp_boundary_preserving_session, app_handler> {
public:
    explicit sess_manager(basic_session_generator <tcp_boundary_preserving_session> *sessionsGenerator)
            : basic_sessions_manager<tcp_boundary_preserving_session, app_handler>(
            sessionsGenerator) {}
};

void netcat_server_main(int port) {
    tcp_boundary_preserving_server server(port);

    sess_manager app(&server);

    while (true) {
        string str;
        getline(cin, str);

        for (auto& session : app.sessions) {
            int res = session.session->send_data({(void*) str.c_str(), (int) str.size()});
            if (res == -1 || res == 0) {
                cerr << "can't send to connection, closing" << endl;
                break;
            }
        }
    }

}


class client_app : public msg_receiver {
public:
    void handle_received_event(received_msg &event) override {
        cout << event.data.get() + event.curr_offset << endl;
    }
};

void netcat_client_main(ip4_addr dest_ip, int port) {
    tcp_boundary_preserving_client client(dest_ip, port, 1212);

    client_app app;
    client.add_listener(&app);

    while (true) {
        string str;
        getline(cin, str);

        int res = client.send_data({(void *) str.c_str(), (int) str.size()});
        if (res == -1 || res == 0) {
            cerr << "can't send to connection, closing" << endl;
            break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) goto print_usage;

    if (argv[1][0] == 'S') {
        if (argc < 3) goto print_usage;
        netcat_server_main(std::stoi(argv[2]));

    } else if (argv[1][0] == 'C') {
        if (argc < 4) goto print_usage;
        netcat_client_main(convert_to_ip4_addr(argv[2]), std::stoi(argv[3]));

    } else goto print_usage;

    return 0;

    print_usage:
    cout << "usage for server: bandwidth S port" << endl;
    cout << "usage for server: bandwidth C server_ip port" << endl;
    return 1;
}