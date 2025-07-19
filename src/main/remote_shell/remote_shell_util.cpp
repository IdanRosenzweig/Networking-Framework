#include "../../direct_connections/tcp/tcp_boundary_preserving_client.h"
#include "../../direct_connections/tcp/tcp_boundary_preserving_server.h"
#include "../../abstract/session/session_manager.h"

#include <string>
#include <iostream>
#include <boost/program_options.hpp>
using namespace std;

#define REMOTE_SHELL_SERVER_PORT 6977
#define REMOTE_SHELL_CLIENT_PORT 5455

class server_app : public session_handler<tcp_boundary_preserving_session_type>, public msg_recv_listener {
public:
    void on_session_start() override { // when session starts, just add this class to its listeners. this calls just receives the messages and echos them
        session.sess_conn->add_listener(this);
    }

    explicit server_app(tcp_boundary_preserving_session_type &&sess) : session_handler(std::move(sess)) {}

private:
    void handle_callback(recv_msg_t &&data) override {
        uint8_t * msg = data.buff_at_curr_offset();
        std::cout << "client's command: " << msg << endl;

        FILE* proc_ptr = popen((const char*) msg, "r");
        if (proc_ptr == nullptr) { // send some error message
            send_msg_t send;
#define ERR_OUTPUT "err"
            int err_len = strlen(ERR_OUTPUT);
            memcpy(send.get_active_buff(), ERR_OUTPUT, err_len);
            send.set_count(err_len);

            session.sess_conn->send_data(std::move(send));
            return;
        }

        send_msg_t send;
        int total_cnt = 0;
        while (true) {
            uint8_t byte;
            if (fread(&byte, 1, 1, proc_ptr) != 1) break;

            send.get_active_buff()[total_cnt++] = byte;
        }
        send.set_count(total_cnt);
        this->session.sess_conn->send_data(std::move(send));
    }
};

using server_sess_manager = session_manager<tcp_boundary_preserving_session_type, server_app>;

void shell_server_main(const string& iface, uint16_t port) {
    tcp_boundary_preserving_server server(port, iface);

    server_sess_manager app(&server);

    while (true) {}

}


class client_app : public msg_recv_listener {
public:
    void handle_callback(recv_msg_t &&data) override {
        std::cout << data.buff_at_curr_offset() << endl;
    }
};

void shell_client_main(const string& iface, ip4_addr server_ip, uint16_t dest_port) {

    tcp_boundary_preserving_client client(server_ip, dest_port, REMOTE_SHELL_CLIENT_PORT, iface);

    client_app app;
    client.add_listener(&app);

    while (true) {
        string str;
        getline(cin, str);

        send_msg_t send;
        memcpy(send.get_active_buff(), str.c_str(), str.size());
        send.set_count(str.size());
        int res = client.send_data(std::move(send));
        if (res == 0 || res == SEND_MEDIUM_ERROR) {
            std::cerr << "err sending data to server" << endl;
        }
    }
}


int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description opts("execute shell commands remotely");
    opts.add_options()
            ("help,h", "print tool use description")
            ("iface", po::value<string>(), "linux interface to use")

            ("client", "use as client")
            ("server", "use as server")

            ("dest", po::value<string>(), "used for client, dest ip address of the server to connect to")
            ("port", po::value<uint16_t>(),
             "if used for client, this is the port that the server listens on. if not specified, uses some default value\nif used on server, this is the port to listen on. if not specified, uses some default value");

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
        uint16_t port;
        if (!vm.count("port")) {
            port = REMOTE_SHELL_SERVER_PORT;
        } else port = vm["port"].as<uint16_t>();

        shell_server_main(iface, port);

    } else {
        uint16_t port;
        if (!vm.count("port")) {
            port = REMOTE_SHELL_SERVER_PORT;
        } else port = vm["port"].as<uint16_t>();

        if (!vm.count("dest")) {
            std::cout << opts << endl;
            return 1;
        }
        string dest = vm["dest"].as<string>();

        shell_client_main(iface, str_to_ip4_addr(dest), port);
    }

}