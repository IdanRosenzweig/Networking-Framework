#pragma once

#include "src/abstract/net_access/net_access.h"

#include "error_codes.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/file.h>

#include <utility>
#include <thread>
#include <optional>
#include <iostream>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;


#define VIRTUAL_NET_MSG_MAX_SZ (0x1000)

optional<int> lock_file(fs::path const& file);
void unlock_file(int fd);

struct virtual_net : public net_access {
public: // todo fix the encapsulation in all classes
    fs::path net_dir;
    fs::path net_lockfile;
    fs::path my_file;

    optional<int> sock_fd = {};

    bool recv_thrd_flag;
    thread recv_thrd;

    shared_ptr<send_medium_bytes> send_access;
    shared_ptr<recv_listener_bytes> recv_access;

    bool connected = false;

public:
    virtual_net() = default;

    bool is_connected() {return connected;}

    void connect(string const& net_name) {
        // network dir
        net_dir = fs::path("/tmp/virtual_net") / net_name;
        fs::create_directories(net_dir);

        // network lockfile
        net_lockfile = net_dir / "lock.file";

        // my own file
        my_file = net_dir / (to_string(getpid()) + ".file");
        unlink(my_file.c_str());

        // open fd for the ipc
        sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock_fd == SOCKET_ERROR) {sock_fd = {}; return;}

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, my_file.string().c_str(), sizeof(addr.sun_path) - 1);
        if (bind(sock_fd.value(), (sockaddr*)&addr, sizeof(addr)) == BIND_ERROR) {close(sock_fd.value()); return;}

        // receiving
        recv_thrd_flag = true;
        recv_thrd = thread([this]() {
            char buff[VIRTUAL_NET_MSG_MAX_SZ];
            while (this->recv_thrd_flag) {
                auto cnt = recv(this->sock_fd.value(), buff, sizeof(buff), 0);
                if (!(cnt > 0)) continue;

                vector<uint8_t> data(buff, buff + cnt);
                if (this->recv_access != nullptr) this->recv_access->handle_recv(data);
            }
        });

        // sending
        struct my_send : public send_medium_bytes {
            virtual_net* par;
            my_send(virtual_net* par) : par(par) {
            }

            err_t send_data(vector<uint8_t> const& data) override {
                // lock dir
                int lock_fd;
                if (auto tmp = lock_file(par->net_lockfile); tmp.has_value()) lock_fd = tmp.value();
                else return err_t::ERR;

                // send to all other nodes in the network
                for (auto const& entry : fs::directory_iterator(par->net_dir)) {
                    fs::path path = entry.path();

                    if (path == par->net_lockfile) continue;
                    if (path == par->my_file) continue;

                    sockaddr_un peer{};
                    peer.sun_family = AF_UNIX;
                    strncpy(peer.sun_path, path.c_str(), sizeof(peer.sun_path) - 1);
                    if (sendto(par->sock_fd.value(), (char*) data.data(), data.size(), 0, (sockaddr*)&peer, sizeof(peer)) == SENDTO_ERROR) {/*cout << "err sending" << endl;*/ continue;}// throw "can't sendto";
                }

                // unlock dir
                unlock_file(lock_fd);

                return err_t::OK;// todo fix this
            }
        };
        send_access = make_shared<my_send>(this);

        // set connected
        connected = true;

        // cout << "connected to the virtual network" << endl;
    }

    void disconnect() {
        // reset recv thrd, and send access
        recv_thrd_flag = false;
        if (recv_thrd.joinable()) recv_thrd.detach();
        send_access = nullptr;

        // reset fd
        if (sock_fd.has_value()) close(sock_fd.value());

        // remove my file
        unlink(my_file.c_str());

        // set disconnected
        connected = false;

        // cout << "disconnected from the virtual network" << endl;
    }

    ~virtual_net() {
        disconnect();
    }

protected:
    shared_ptr<send_medium_bytes> impl_get_send_access() override {
        return send_access;
    }

    void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
        recv_access = recv;
    }
};


