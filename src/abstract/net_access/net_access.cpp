#include "net_access.h"


shared_ptr<net_access_send> net_access_send_empty() {
    struct my_net_access_send : public net_access_send {
    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return nullptr;
        }
    };

    return make_shared<my_net_access_send>();
}

shared_ptr<net_access_send> net_access_send_make_static(shared_ptr<send_medium_bytes>&& static_send) {
    struct my_net_access_send : public net_access_send {
    private:
        shared_ptr<send_medium_bytes> static_send;

    public:
        my_net_access_send(shared_ptr<send_medium_bytes>&& _static_send) : static_send(std::move(_static_send)) {
        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return static_send;
        }
    };

    return make_shared<my_net_access_send>(std::move(static_send));
}


shared_ptr<net_access_recv> net_access_recv_empty() {
    struct my_net_access_recv : public net_access_recv {
    protected:
        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            // do nothing
        }
    };

    return make_shared<my_net_access_recv>();
}


shared_ptr<net_access> net_access_make_empty() {
    struct my_net_access : public net_access {
    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            return nullptr;
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            // do nothing
        }
    };

    return make_shared<my_net_access>();
}

shared_ptr<net_access> net_access_make_from_send_recv(shared_ptr<net_access_send>&& send, shared_ptr<net_access_recv>&& recv) {
    struct my_net_access : public net_access {
    private:
        shared_ptr<net_access_send> base_send;
        shared_ptr<net_access_recv> base_recv;

    public:
        my_net_access(shared_ptr<net_access_send>&& _send, shared_ptr<net_access_recv>&& _recv) : base_send(std::move(_send)), base_recv(std::move(_recv)) {
        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            // use the send
            return base_send->get_send_access();
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            // use the recv
            base_recv->set_recv_access(recv);
        }
    };

    return make_shared<my_net_access>(std::move(send), std::move(recv));
}

shared_ptr<net_access> net_access_override_send(shared_ptr<net_access>&& net_access, shared_ptr<net_access_send>&& send) {
    struct my_net_access : public net_access {
    private:
        shared_ptr<net_access> base_access;
        shared_ptr<net_access_send> overridden_send;

    public:
        my_net_access(shared_ptr<net_access>&& _base_access, shared_ptr<net_access_send>&& _overridden_send) : base_access(std::move(_base_access)), overridden_send(std::move(_overridden_send)) {
        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            // use the overridden send
            return overridden_send->get_send_access();
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            // use the base access
            base_access->set_recv_access(recv);
        }
    };

    return make_shared<my_net_access>(std::move(net_access), std::move(send));
}

shared_ptr<net_access> net_access_override_recv(shared_ptr<net_access>&& net_access, shared_ptr<net_access_recv>&& recv) {
    struct my_net_access : public net_access {
    private:
        shared_ptr<net_access> base_access;
        shared_ptr<net_access_recv> overridden_recv;

    public:
        my_net_access(shared_ptr<net_access>&& _base_access, shared_ptr<net_access_recv>&& _overridden_recv) : base_access(std::move(_base_access)), overridden_recv(std::move(_overridden_recv)) {
        }

    protected:
        shared_ptr<send_medium_bytes> impl_get_send_access() override {
            // use the base access
            return base_access->get_send_access();
        }

        void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
            // use the overridden recv
            overridden_recv->set_recv_access(recv);
        }
    };

    return make_shared<my_net_access>(std::move(net_access), std::move(recv));
}
