#ifndef NETWORKING_BASIC_SEND_MEDIUM_H
#define NETWORKING_BASIC_SEND_MEDIUM_H

template <typename TYPE>
class basic_send_medium {
public:
    // this function is called to send
#define SEND_MEDIUM_ERROR (-1)
    virtual int send_data(TYPE&& val) = 0;

    virtual ~basic_send_medium() = default;
};
#endif //NETWORKING_BASIC_SEND_MEDIUM_H
