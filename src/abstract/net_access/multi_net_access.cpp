#include "multi_net_access.h"

vector<shared_ptr<net_access_recv>> multi_net_access_recv_split(shared_ptr<net_access_recv> const& base, int no_splits) {
    // create the multiplexer    
    shared_ptr<multi_recv_listener_bytes> multiplexer = make_shared<multi_recv_listener_bytes>();
    base->set_recv_access(multiplexer);

    // split
    vector<shared_ptr<net_access_recv>> res(no_splits);
    for (int i = 0; i < no_splits; i++) {
        struct my_net_access_recv : public net_access_recv {
        private:
            shared_ptr<multi_recv_listener_bytes> par;
            shared_ptr<recv_listener_bytes> curr = nullptr;

        public:
            my_net_access_recv(shared_ptr<multi_recv_listener_bytes> const& par) : par(par) {
            }

        protected:
            void impl_set_recv_access(shared_ptr<recv_listener_bytes> const& recv) override {
                if (curr != nullptr) par->remove_listener(curr);

                par->add_recv_listener(recv);
                curr = recv;
            }
        };
        
        res[i] = make_shared<my_net_access_recv>(multiplexer);
    }

    return res;
}