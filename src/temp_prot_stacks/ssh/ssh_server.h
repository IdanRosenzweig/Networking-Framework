#ifndef SERVERCLIENT_SSH_SERVER_H
#define SERVERCLIENT_SSH_SERVER_H

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>
#include <libssh/legacy.h>
#include <libssh/libssh_version.h>
#include <libssh/sftp.h>
#include <libssh/ssh2.h>

#include <vector>
#include <memory>

#include "../../abstract/session/session_conn.h"

#include "ssh_conn_session.h"

class client_session : public session_conn {
    ssh_session session;
    ssh_channel raw_channel;

    std::thread worker;

public:
    explicit client_session(ssh_session session) : session(session) {
        raw_channel = ssh_channel_new(session);
        if (raw_channel == nullptr) {
            std::cerr << "err creating raw_channel" << endl;
            return;
        }

        if (ssh_channel_open_session(raw_channel) != SSH_OK) {
            std::cerr << "err opening session raw_channel" << endl;
            ssh_channel_free(raw_channel);
            return;
        }

        worker = std::thread([&]() -> void {
#define BUFF_SZ 1024
            uint8_t buff[BUFF_SZ];
            while (true) {
                memset(buff, 0, BUFF_SZ);

                int data_sz = ssh_channel_read(raw_channel, buff, BUFF_SZ, false);
                if (data_sz <= 0) continue;

                received_msg msg;
                msg.data = udata_t(data_sz, 0x00);
                memcpy(msg.data.data(), buff, data_sz);
                msg.curr_offset = 0;
                this->handle_received_event(msg);
            }
        });
    }

    ~client_session() {
        if (worker.joinable()) worker.detach();

        ssh_channel_send_eof(raw_channel);
        ssh_channel_close(raw_channel);
        ssh_channel_free(raw_channel);

        ssh_disconnect(session);
        ssh_free(session);
    }

    int send_data(send_msg<>&& val) override {
        return ssh_channel_write(raw_channel, val.buff, val.count);
    }

};

class ssh_server {
    ssh_bind sshbind;
    std::thread worker;

public:

    std::vector<std::unique_ptr<session_conn>> as_server_sessions; // raw_session where i am the aggregator

    ssh_server(int port) {
        sshbind = ssh_bind_new();
        if (sshbind == nullptr) {
            std::cerr << "err creating SSH bind" << endl;
            throw 1;
        }

        if (ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0") < 0) {
            std::cerr << "err setting bind address" << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }

        if (ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port) < 0) {
            std::cerr << "err setting bind port" << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }

        if (ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, "/etc/ssh/ssh_host_rsa_key") < 0) {
            std::cerr << "err setting rsakey" << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }

        if (ssh_bind_listen(sshbind) < 0) {
            std::cerr << "err listening to socket: " << ssh_get_error(sshbind) << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }


        ssh_session session = ssh_new();
        if (session == nullptr) {
            std::cerr << "err creating session" << endl;
            return;
        }

        std::cout << "waiting for accept" << endl;
        if (ssh_bind_accept(sshbind, session) != SSH_OK) {
            std::cerr << "err accepting incoming connection" << endl;
            ssh_free(session);
            return;
        }

        if (ssh_handle_key_exchange(session) != SSH_OK) {
            std::cerr << "err ssh_handle_key_exchange: " << ssh_get_error(session) << endl;
            return;
        }
        std::cout << "accpeted" << endl;




        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD);

        int auth_methods = ssh_userauth_list(session, nullptr);
        if (!(auth_methods & SSH_AUTH_METHOD_PASSWORD)) {
            fprintf(stderr, "Server only supports password authentication\n");
            throw 1;
        }

        std::cout << "waiting for auth" << endl;
        std::string password = "123"; // Replace with actual password logic
        if (ssh_userauth_password(session, nullptr, password.c_str()) != SSH_AUTH_SUCCESS) {
            fprintf(stderr, "Authentication failed\n");
            throw 1;
        }

        std::cout << "creating new session" << endl;
        as_server_sessions.push_back(std::make_unique<client_session>(session));

    }

    ~ssh_server() {
//        worker.detach();
//        ssh_bind_free(sshbind);

    }


};


#endif //SERVERCLIENT_SSH_SERVER_H
