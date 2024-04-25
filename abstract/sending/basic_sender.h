#ifndef SERVERCLIENT_BASIC_SENDER_H
#define SERVERCLIENT_BASIC_SENDER_H

template <typename T>
class basic_sender {
public:
    // this function is called to send data
    virtual int send_data(T val) = 0;
};
#endif //SERVERCLIENT_BASIC_SENDER_H
