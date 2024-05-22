#include "../../temp_prot_stacks/tcp_client_server/tcp_boundary_preserving_client.h"
#include "../../temp_prot_stacks/tcp_client_server/tcp_boundary_preserving_server.h"
#include "../../abstract/session/session_manager.h"

#include <boost/program_options.hpp>
#include <unistd.h>
#include <iostream>

using namespace std;

class app_handler : public session_handler<tcp_boundary_preserving_session>, public msg_receiver {
public:
    void on_session_start() override { // when session starts, just add this class to its listeners. this calls just receives the messages and echos them
        session.session->add_listener(this);
    }

    explicit app_handler(session_t<tcp_boundary_preserving_session> &session) : session_handler(session) {}

private:
    void handle_received_event(received_msg &&event) override {
        cout << event.data.data() + event.curr_offset << endl;
//        session->send_data({event.data_t.get() + event.curr_offset, event.data.size() - event.curr_offset});
    }
};

class sess_manager : public session_manager<tcp_boundary_preserving_session, app_handler> {
public:
    explicit sess_manager(session_generator<tcp_boundary_preserving_session> *sessionsGenerator)
            : session_manager<tcp_boundary_preserving_session, app_handler>(
            sessionsGenerator) {}
};

void netcat_server_main(const string& iface, int port) {
    tcp_boundary_preserving_server server(port);

    sess_manager app(&server);

    while (true) {
        string str;
        getline(cin, str);

        for (auto &session: app.sessions) {
            send_msg send;
            memcpy(send.get_active_buff(), str.c_str(), str.size());
            send.set_count(str.size());
            int res = session.session.session->send_data(std::move(send));
            if (res == -1 || res == 0) {
                cerr << "can't send to connection, closing" << endl;
                break;
            }
        }
    }

}


class client_app : public msg_receiver {
public:
    void handle_received_event(received_msg &&event) override {
        cout << event.data.data() + event.curr_offset << endl;
    }
};

void netcat_client_main(const string& iface, ip4_addr dest_ip, int port) {
    tcp_boundary_preserving_client client(dest_ip, port, 1212);

    client_app app;
    client.add_listener(&app);

    while (true) {
        string str;
        getline(cin, str);

        send_msg send;
        memcpy(send.get_active_buff(), str.c_str(), str.size());
        send.set_count(str.size());
        int res = client.send_data(std::move(send));
        if (res == -1 || res == 0) {
            cerr << "can't send to connection, closing" << endl;
            break;
        }
    }
}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("Allowed options");
    opts.add_options()
            ("help", "print tool use description")
            ("interface,i", po::value<string>(), "linux interface to use")

            ("client", "use as client")
            ("server", "use as server")

            ("dest", po::value<string>(), "used for client, dest ip address of the server to connect to")
            ("port", po::value<int>(),
             "if used for client, this is the port that the server listens on.\nif used on server. this is the port to listen on");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << opts << endl;
        return 1;
    }

    if (!vm.count("interface")) {
        cout << opts << endl;
        return 1;
    }
    string iface = vm["interface"].as<string>();

    bool server;
    if (vm.count("server")) server = true;
    else if (vm.count("client")) server = false;
    else {
        cout << opts << endl;
        return 1;
    }

    if (server) {
        if (!vm.count("port")) {
            cout << opts << endl;
            return 1;
        }
        int port = vm["port"].as<int>();

        netcat_server_main(iface, port);

    } else {
        if (!vm.count("port")) {
            cout << opts << endl;
            return 1;
        }
        int port = vm["port"].as<int>();

        if (!vm.count("dest")) {
            cout << opts << endl;
            return 1;
        }
        string dest = vm["dest"].as<string>();

        netcat_client_main(iface, convert_to_ip4_addr(dest), port);
    }
}