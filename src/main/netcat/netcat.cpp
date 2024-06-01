#include "../../direct_connections/tcp/tcp_boundary_preserving_client.h"
#include "../../direct_connections/tcp/tcp_boundary_preserving_server.h"
#include "../../abstract/session/session_manager.h"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;

class server_app : public session_handler<tcp_boundary_preserving_session_type>, public msg_recv_listener {
public:
    void on_session_start() override { // when session starts, just add this class to its listeners. this calls just receives the messages and echos them
        session.sess_conn->add_listener(this);
    }

    explicit server_app(tcp_boundary_preserving_session_type &&session) : session_handler(std::move(session)) {}

private:
    void handle_callback(recv_msg_t &&data) override {
        uint8_t * msg = data.buff_at_curr_offset();
        std::cout << msg << endl;
    }
};

using server_sess_manager = session_manager<tcp_boundary_preserving_session_type, server_app>;

void netcat_server_main(const string& iface, uint16_t port) {
    // listening and generating the session
    tcp_boundary_preserving_server server(port);

    // handling and storing the sessions
    server_sess_manager app(&server);

    while (true) {
        string str;
        getline(cin, str);

        auto it = app.sessions.begin();
        while (it != app.sessions.end()) {
            send_msg_t send;
            memcpy(send.get_active_buff(), str.c_str(), str.size());
            send.set_count(str.size());

            int res = (*it)->handler.session.sess_conn->send_data(std::move(send));
            if (res == SEND_MEDIUM_ERROR || res == 0) { // can't send to session, remove it
                it = app.sessions.erase(it);
            } else it++; // can send, just advance the iterator
        }

    }

}


class client_app : public msg_recv_listener {
public:
    void handle_callback(recv_msg_t &&data) override {
        std::cout << data.buff_at_curr_offset() << endl;
    }
};

void netcat_client_main(const string& iface, ip4_addr dest_ip, uint16_t port) {
    tcp_boundary_preserving_client client(dest_ip, port, 1212);

    client_app app;
    client.add_listener(&app);

    while (true) {
        string str;
        getline(cin, str);

        send_msg_t send;
        memcpy(send.get_active_buff(), str.c_str(), str.size());
        send.set_count(str.size());
        int res = client.send_data(std::move(send));
        if (res == SEND_MEDIUM_ERROR || res == 0) {
            std::cerr << "can't send buff to server" << endl;
        }
    }
}


int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opts("send and receive raw text");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")

            ("client", "use as client")
            ("server", "use as server")

            ("dest", po::value<string>(), "used for client, dest ip address of the server to connect to")
            ("port", po::value<uint16_t>(),
             "if used for client, this is the port that the server listens on.\nif used on server. this is the port to listen on");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << endl;
        return 1;
    }

    if (!vm.count("iface")) {
        std::cout << opts << endl;
        return 1;
    }
    string iface = vm["iface"].as<string>();

    bool server;
    if (vm.count("server")) server = true;
    else if (vm.count("client")) server = false;
    else {
        std::cout << opts << endl;
        return 1;
    }

    if (server) {
        if (!vm.count("port")) {
            std::cout << opts << endl;
            return 1;
        }
        uint16_t port = vm["port"].as<uint16_t>();

        netcat_server_main(iface, port);

    } else {
        if (!vm.count("port")) {
            std::cout << opts << endl;
            return 1;
        }
        uint16_t port = vm["port"].as<uint16_t>();

        if (!vm.count("dest")) {
            std::cout << opts << endl;
            return 1;
        }
        string dest = vm["dest"].as<string>();

        netcat_client_main(iface, convert_to_ip4_addr(dest), port);
    }

}