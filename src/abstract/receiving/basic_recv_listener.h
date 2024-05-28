#ifndef SERVERCLIENT_BASIC_RECV_LISTENER_H
#define SERVERCLIENT_BASIC_RECV_LISTENER_H

// receiving data is different from sending data in the sense that
// you can control when to send msg, but receiving msg is rather interrupted.

// regarding network protocols, when sending a msg you don't have to worry about the protocol stack (you control it),
// but when receiving a msg you have to process the protocol stack and analyze it

// a basic network api for handling interrupted data
template <typename T>
class basic_recv_listener {
public:
    // this function would be interrupted when receiving msg
    virtual void handle_received_event(T&& event) = 0;
};

#endif //SERVERCLIENT_BASIC_RECV_LISTENER_H
