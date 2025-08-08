#include "net_access.h"

shared_ptr<net_access> net_access_save_send_access(shared_ptr<net_access>&& net_access, shared_ptr<send_medium_bytes>&& send) {
    struct my_net_access : public net_access {
        shared_ptr<net_access> access;
        shared_ptr<send_medium_bytes> send;

        my_net_access(shared_ptr<net_access>&& access, shared_ptr<send_medium_bytes>&& send) : access(std::move(access)), send(std::move(send)) {
        }

        /* send access, provided by the iface access, can be used outside */
    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return send;
        }

        /* recv access, can be set from outside in order to receive from the iface */
    protected:
        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            access->set_recv_access(recv);
        }
    };

    return make_shared<my_net_access>(std::move(net_access), std::move(send));
}