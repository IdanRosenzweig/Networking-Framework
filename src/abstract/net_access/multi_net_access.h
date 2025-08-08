#pragma once

#include "net_access.h"
#include "src/abstract/receiving/multi_recv_listener.h"

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

    multi_net_access(multi_net_access&& other) : access(std::move(other.access)), multi_recv(std::move(other.multi_recv)) {
    }

    multi_net_access& operator=(multi_net_access const& other) = delete;

    multi_net_access& operator=(multi_net_access&& other) {
        access = std::move(other.access);
        multi_recv = std::move(other.multi_recv);
        return *this;
    }

    shared_ptr<net_access> generate_net_access() {
        struct my_net_access : net_access {
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