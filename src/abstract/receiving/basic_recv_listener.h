#ifndef NETWORKING_BASIC_RECV_LISTENER_H
#define NETWORKING_BASIC_RECV_LISTENER_H

// receiving buff is different from sending buff in the sense that
// you can control when to send plain_data, but receiving plain_data is rather interrupted.

// regarding network protocols, when sending a plain_data you don't have to worry about the protocol stack (you control it),
// but when receiving a plain_data you have to process the protocol stack and analyze it

// a basic network api for handling callback buff
template <typename T>
class basic_recv_listener {
public:
    // this callback function would be called when buff is received
    virtual void handle_callback(T&& data) = 0;
};

#endif //NETWORKING_BASIC_RECV_LISTENER_H
