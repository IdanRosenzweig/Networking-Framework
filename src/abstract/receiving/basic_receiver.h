#ifndef SERVERCLIENT_BASIC_RECEIVER_H
#define SERVERCLIENT_BASIC_RECEIVER_H

// receiving msg is different from sending msg in the sense that
// you can control when to send msg, but receiving msg is rather interrupted.

// in addition, when sending msg you don't have to worry about the protocol stack (you control it),
// but when receiving msg you have to process the protocol stack

// a basic network app_handler for handling interrupted msg
template <typename T>
class basic_receiver {
public:
    // this function would be interrupted when receiving msg
    virtual void handle_received_event(T& event) = 0;
};

#endif //SERVERCLIENT_BASIC_RECEIVER_H
