#pragma once

#include "net_access_bytes.h"
#include "src/abstract/receiving/multi_recv_listener.h"

struct multi_net_access {
public:
    shared_ptr<net_access_bytes> net_access;
    shared_ptr<multi_recv_listener<vector<uint8_t>>> multi_recv;

public:
    multi_net_access() = delete;
    multi_net_access(shared_ptr<net_access_bytes>&& _net_access) : net_access(std::move(_net_access)), multi_recv(make_shared<multi_recv_listener<vector<uint8_t>>>()) {
        net_access->set_recv_access(multi_recv);
    }

    shared_ptr<net_access_bytes> generate_net_access() {
        struct my_net_access : net_access_bytes {
        private:
            multi_net_access* par;
            shared_ptr<basic_recv_listener<vector<uint8_t>>> curr_recv;

        public:
            my_net_access(multi_net_access* par) : par(par) {
            }

            ~my_net_access() {
                if (curr_recv != nullptr) par->multi_recv->remove_listener(curr_recv);
            }
            
        protected:
            shared_ptr<basic_send_medium<vector<uint8_t>>> impl_get_send_access() override {
                return par->net_access->get_send_access();
            }

            void impl_set_recv_access(shared_ptr<basic_recv_listener<vector<uint8_t>>> const& recv) override {
                if (curr_recv != nullptr) par->multi_recv->remove_listener(curr_recv);

                par->multi_recv->add_recv_listener(recv);
                curr_recv = recv;
            }
        };

        return make_shared<my_net_access>(this);
    }
};