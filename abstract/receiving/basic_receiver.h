#ifndef SERVERCLIENT_BASIC_RECEIVER_H
#define SERVERCLIENT_BASIC_RECEIVER_H

// receiving data is different from sending data in the sense that
// you can control when to send data, but receiving data is rather interrupted.

// in addition, when sending data you don't have to worry about the protocol stack (you control it),
// but when receiving data you have to process the protocol stack

// a basic network handler for handling interrupted data
template <typename T>
class basic_receiver {
public:
    // this function would be interrupted when receiving data
    virtual void handle_received_event(T event) = 0;
};

#endif //SERVERCLIENT_BASIC_RECEIVER_H
