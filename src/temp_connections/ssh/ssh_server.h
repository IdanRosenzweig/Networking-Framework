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

#include "../../abstract/connection/basic_session.h"

#include "ssh_conn_session.h"

class client_session : public basic_session {
    ssh_session session;
    ssh_channel raw_channel;

    std::thread worker;

public:
    explicit client_session(ssh_session session) : session(session) {
        raw_channel = ssh_channel_new(session);
        if (raw_channel == nullptr) {
            cerr << "err creating raw_channel" << endl;
            return;
        }

        if (ssh_channel_open_session(raw_channel) != SSH_OK) {
            cerr << "err opening session raw_channel" << endl;
            ssh_channel_free(raw_channel);
            return;
        }

        worker = std::thread([&]() -> void {
#define BUFF_SZ 1024
            char buff[BUFF_SZ];
            while (true) {
                memset(buff, 0, BUFF_SZ);

                int data_sz = ssh_channel_read(raw_channel, buff, BUFF_SZ, false);
                if (data_sz <= 0) continue;

                uint8_t *alloc_msg = new uint8_t[data_sz];
                memcpy(alloc_msg, buff, data_sz);

                received_msg msg;
                msg.data = unique_ptr<uint8_t>(alloc_msg);
                msg.sz = data_sz;
                msg.curr_offset = 0;
                this->handle_received_event(msg);
            }
        });
    }

    ~client_session() {
        worker.detach();

        ssh_channel_send_eof(raw_channel);
        ssh_channel_close(raw_channel);
        ssh_channel_free(raw_channel);

        ssh_disconnect(session);
        ssh_free(session);
    }

    int send_data(send_msg val) override {
        return ssh_channel_write(raw_channel, val.buff, val.count);
    }

};

class ssh_server {
    ssh_bind sshbind;
    std::thread worker;

public:

    std::vector<std::unique_ptr<basic_session>> as_server_sessions; // sessions where i am the daemon

    ssh_server(int port) {
        sshbind = ssh_bind_new();
        if (sshbind == nullptr) {
            cerr << "err creating SSH bind" << endl;
            throw 1;
        }

        if (ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDADDR, "0.0.0.0") < 0) {
            cerr << "err setting bind address" << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }

        if (ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_BINDPORT, &port) < 0) {
            cerr << "err setting bind port" << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }

        if (ssh_bind_options_set(sshbind, SSH_BIND_OPTIONS_RSAKEY, "/etc/ssh/ssh_host_rsa_key") < 0) {
            cerr << "err setting rsakey" << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }

        if (ssh_bind_listen(sshbind) < 0) {
            cerr << "err listening to socket: " << ssh_get_error(sshbind) << endl;
            ssh_bind_free(sshbind);
            throw 1;
        }


        ssh_session session = ssh_new();
        if (session == nullptr) {
            cerr << "err creating session" << endl;
            return;
        }

        cout << "waiting for accept" << endl;
        if (ssh_bind_accept(sshbind, session) != SSH_OK) {
            cerr << "err accepting incoming connection" << endl;
            ssh_free(session);
            return;
        }

        if (ssh_handle_key_exchange(session) != SSH_OK) {
            cerr << "err ssh_handle_key_exchange: " << ssh_get_error(session) << endl;
            return;
        }
        cout << "accpeted" << endl;




        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD);

        int auth_methods = ssh_userauth_list(session, nullptr);
        if (!(auth_methods & SSH_AUTH_METHOD_PASSWORD)) {
            fprintf(stderr, "Server only supports password authentication\n");
            throw 1;
        }

        cout << "waiting for auth" << endl;
        std::string password = "123"; // Replace with actual password logic
        if (ssh_userauth_password(session, nullptr, password.c_str()) != SSH_AUTH_SUCCESS) {
            fprintf(stderr, "Authentication failed\n");
            throw 1;
        }

        cout << "creating new session" << endl;
        as_server_sessions.push_back(std::make_unique<client_session>(session));

    }

    ~ssh_server() {
//        worker.detach();
//        ssh_bind_free(sshbind);

    }


};


#endif //SERVERCLIENT_SSH_SERVER_H
