#ifndef NETWORKING_BASIC_SEND_MEDIUM_H
#define NETWORKING_BASIC_SEND_MEDIUM_H

template <typename TYPE>
class basic_send_medium {
public:
    // call this function send buff
#define SEND_MEDIUM_ERROR (-1)
    virtual int send_data(TYPE&& data) = 0;

    virtual ~basic_send_medium() = default;
};
#endif //NETWORKING_BASIC_SEND_MEDIUM_H
