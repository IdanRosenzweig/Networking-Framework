#include "../../temp_connections/tcp_client_server/tcp_boundary_preserving_client.h"
#include "../../temp_connections/tcp_client_server/tcp_boundary_preserving_server.h"
#include "../../abstract/session/basic_sessions_manager.h"

#include <boost/program_options.hpp>
#include <unistd.h>
#include <iostream>
using namespace std;

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
            send_msg send{(void*) str.c_str(), (int) str.size()};
            int res = session.session->send_data(send);
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

        send_msg send{(void *) str.c_str(), (int) str.size()};
        int res = client.send_data(send);
        if (res == -1 || res == 0) {
            cerr << "can't send to connection, closing" << endl;
            break;
        }
    }
}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "print tool use description")
            ("client", "use as client")
            ("server", "use as server")
            ("dest", po::value<string>(),"used for client, dest ip address of the server to connect to")
            ("port", po::value<int>(), "if used for client, this is the port that the server listens on.\nif used on server. this is the port to listen on");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    bool server;
    if (vm.count("server")) server = true;
    else if (vm.count("client")) server = false;
    else {
        cout << desc << endl;
        return 1;
    }

    if (server) {
        if (!vm.count("port")) {
            cout << desc << endl;
            return 1;
        }
        int port = vm["port"].as<int>();

        netcat_server_main(port);

    } else {
        if (!vm.count("port")) {
            cout << desc << endl;
            return 1;
        }
        int port = vm["port"].as<int>();

        if (!vm.count("dest")) {
            cout << desc << endl;
            return 1;
        }
        string dest = vm["dest"].as<string>();

        netcat_client_main(convert_to_ip4_addr(dest), port);
    }
}