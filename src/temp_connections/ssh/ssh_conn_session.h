#ifndef SERVERCLIENT_SSH_CONN_SESSION_H
#define SERVERCLIENT_SSH_CONN_SESSION_H

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#include "../../abstract/sending/msg_sender.h"
#include "../../abstract/receiving/msg_receiver.h"
#include "../../abstract/connection/basic_connection.h"

#include <thread>

// ssh runs over ip and tcp_client_server
class ssh_conn_session : public basic_connection {
    ssh_session session;
    ssh_channel raw_channel;

    std::thread worker;
public:
    ssh_conn_session(const string &host, const string &user, const string &pass, int port = 22) {
        session = ssh_new();
        if (session == nullptr)
            exit(-1);

        ssh_options_set(session, SSH_OPTIONS_HOST, host.c_str());
        ssh_options_set(session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(session, SSH_OPTIONS_USER, user.c_str());
        int verbosity = SSH_LOG_PROTOCOL;
        ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);


        cout << "conencting..." << endl;
        if (ssh_connect(session) != SSH_OK) {
            cerr << "error connection to host: " << host.c_str() << ", " << ssh_get_error(session) << endl;
            exit(-1);
        }
        cout << "connected" << endl;

//        ssh_set_auth_methods(session, SSH_AUTH_METHOD_PASSWORD);
        sleep(2);

//        while (true) {
            std::string password = "123"; // Replace with actual password logic
            if (ssh_userauth_password(session, nullptr, password.c_str()) != SSH_AUTH_SUCCESS) {
                cerr << "Authentication failed: " << ssh_get_error(session) << endl;
                throw;
//                return;
            }
//            return;
//        }

        cout << "authenticated" << endl;


        raw_channel = ssh_channel_new(session);
        if (raw_channel == nullptr) {
            cerr << "err opening raw_channel" << endl;
            throw SSH_ERROR;
        }

        if (ssh_channel_open_session(raw_channel) != SSH_OK) {
            cerr << "err opening raw_channel session" << endl;
            ssh_channel_free(raw_channel);
            throw;
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

    ~ssh_conn_session() {
        if (worker.joinable()) worker.detach();

        ssh_channel_send_eof(raw_channel);
        ssh_channel_close(raw_channel);
        ssh_channel_free(raw_channel);

        ssh_disconnect(session);
        ssh_free(session);
    }

//    void execute_remote_cli_command(const string& command) {
//        ssh_channel raw_channel = ssh_channel_new(session);
//        if (raw_channel == nullptr) {
//            cerr << "err opening raw_channel" << endl;
//            throw SSH_ERROR;
//        }
//
//        if (ssh_channel_open_session(raw_channel) != SSH_OK) {
//            cerr << "err opening raw_channel session" << endl;
//            ssh_channel_free(raw_channel);
//            throw;
//        }
//
//
//        if (ssh_channel_request_exec(raw_channel, command.c_str()) != SSH_OK) {
//            cerr << "err executing command" << endl;
//            ssh_channel_close(raw_channel);
//            ssh_channel_free(raw_channel);
//            throw;
//        }
//
//#define BUFF_SZ 1024
//        char buff[BUFF_SZ];
//        memset(buff, 0, BUFF_SZ);
//        while (true) {
//            int cnt = ssh_channel_read(raw_channel, buff, BUFF_SZ, 0);
//
//            if (cnt < 0) {
//                cerr << "err reading back" << endl;
//                ssh_channel_close(raw_channel);
//                ssh_channel_free(raw_channel);
//                throw SSH_ERROR;
//            } else if (cnt == 0) break;
//
//            if (write(1, buff, cnt) != cnt) {
//                cerr << "err printing result" << endl;
//                ssh_channel_close(raw_channel);
//                ssh_channel_free(raw_channel);
//                throw SSH_ERROR;
//            }
//        }
//
//        ssh_channel_send_eof(raw_channel);
//        ssh_channel_close(raw_channel);
//        ssh_channel_free(raw_channel);
//    }

    int send_data(send_msg val) override;

};


#endif //SERVERCLIENT_SSH_CONN_SESSION_H
