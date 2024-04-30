#ifndef SERVERCLIENT_MULTIPLEXER_H
#define SERVERCLIENT_MULTIPLEXER_H

#include <map>

template <typename KEY, typename VAL>
class multiplexer {
    std::map<KEY, VAL> handlers;

public:
    void assign_to_key(KEY key, VAL val) {
        handlers[key] = val;
    }

    VAL& get_val_of_key(KEY key) {
        return handlers[key];
    }

    bool is_key_assigned(KEY key) {
        return handlers.count(key);
    }
};

#endif //SERVERCLIENT_MULTIPLEXER_H
