#pragma once

#include "net_access.h"
#include "src/abstract/receiving/multi_recv_listener.h"

struct multi_net_access_send {
private:
    shared_ptr<net_access_send> send;

public:
    multi_net_access_send() = delete;
    multi_net_access_send(shared_ptr<net_access_send>&& _send) : send(std::move(_send)) {
    }

    multi_net_access_send(multi_net_access_send const& other) = delete;

    multi_net_access_send(multi_net_access_send&& other) : send(std::move(other.send)) {
    }

    multi_net_access_send& operator=(multi_net_access_send const& other) = delete;

    multi_net_access_send& operator=(multi_net_access_send&& other) {
        send = std::move(other.send);
        return *this;
    }

    shared_ptr<net_access_send> generate_net_access_send() {
        return net_access_send_make_static(
            shared_ptr<send_medium_bytes>(send->get_send_access())
        );
    }
};


struct multi_net_access_recv {
public:
    shared_ptr<net_access_recv> recv;
    shared_ptr<multi_recv_listener_bytes> multiplexer;

public:
    multi_net_access_recv() = delete;
    multi_net_access_recv(shared_ptr<net_access_recv>&& _recv) : recv(std::move(_recv)), multiplexer(make_shared<multi_recv_listener_bytes>()) {
        recv->set_recv_access(multiplexer);
    }

    multi_net_access_recv(multi_net_access_send const& other) = delete;
    multi_net_access_recv(multi_net_access_send&& other) = delete;

    multi_net_access_recv& operator=(multi_net_access_recv const& other) = delete;
    multi_net_access_recv& operator=(multi_net_access_recv&& other) = delete;

    shared_ptr<net_access_recv> generate_net_access_send() {
        struct my_net_access_recv : public net_access_recv {
        private:
            multi_net_access_recv* par;
            shared_ptr<recv_listener_bytes> curr = nullptr;

        public:
            my_net_access_recv(multi_net_access_recv* par) : par(par) {
            }

        protected:
            void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
                if (curr != nullptr) par->multiplexer->remove_listener(curr);

                par->multiplexer->add_recv_listener(recv);
                curr = recv;
            }
        };

        return make_shared<my_net_access_recv>(this);
    }
};

vector<shared_ptr<net_access_recv>> multi_net_access_recv_split(shared_ptr<net_access_recv> const& base, int no_splits);


struct multi_net_access {
public:
    shared_ptr<net_access> access;
    shared_ptr<multi_recv_listener_bytes> multi_recv;

public:
    multi_net_access() = delete;
    multi_net_access(shared_ptr<net_access>&& _access) : access(std::move(_access)), multi_recv(make_shared<multi_recv_listener_bytes>()) {
        access->set_recv_access(multi_recv);
    }

    multi_net_access(multi_net_access const& other) = delete;
    multi_net_access(multi_net_access&& other) = delete;

    multi_net_access& operator=(multi_net_access const& other) = delete;
    multi_net_access& operator=(multi_net_access&& other) = delete;

    shared_ptr<net_access> generate_net_access() {
        struct my_net_access : public net_access {
            multi_net_access* par;
            my_net_access(multi_net_access* par) : par(par) {
            }

            ~my_net_access() {
                if (curr_recv != nullptr) par->multi_recv->remove_listener(curr_recv);
            }
            
        protected:
            shared_ptr<send_medium_bytes> impl_get_send_access() override {
                return par->access->get_send_access();
            }

            shared_ptr<recv_listener_bytes> curr_recv;
            void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
                if (curr_recv != nullptr) par->multi_recv->remove_listener(curr_recv);

                par->multi_recv->add_recv_listener(recv);
                curr_recv = recv;
            }
        };

        return make_shared<my_net_access>(this);
    }
};