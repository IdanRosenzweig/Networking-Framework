#ifndef SERVERCLIENT_BASIC_FORWARDER_TARGET_H
#define SERVERCLIENT_BASIC_FORWARDER_TARGET_H

template <typename T>
class basic_forwarder_target {
public:
    virtual void handle_forwarding(T&& msg) = 0;
};
#endif //SERVERCLIENT_BASIC_FORWARDER_TARGET_H
