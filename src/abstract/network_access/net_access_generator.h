#pragma once

#include <functional>
#include <memory>
#include <cstdint>
using namespace std;

#include "net_access_bytes.h"

struct net_access_generator_listener {
public:
    virtual ~net_access_generator_listener() = default;

    virtual void handle_new_net_access(shared_ptr<net_access_bytes> const& net_access) = 0;
};

struct net_access_generator {
protected:
    shared_ptr<net_access_generator_listener> listener;

public:
    virtual ~net_access_generator() = default;

    void set_net_access_generator_listener(shared_ptr<net_access_generator_listener> const& listener) {
        this->listener = listener;
    }
};
