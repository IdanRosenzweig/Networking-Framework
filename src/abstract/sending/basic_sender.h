#ifndef SERVERCLIENT_BASIC_SENDER_H
#define SERVERCLIENT_BASIC_SENDER_H

template <typename TYPE>
class basic_sender {
public:
    // this function is called to send
    virtual int send_data(TYPE&& val) = 0;

};
#endif //SERVERCLIENT_BASIC_SENDER_H
