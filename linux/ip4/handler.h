#ifndef SERVERCLIENT_HANDLER_H
#define SERVERCLIENT_HANDLER_H

class encapsulated_data_handler {
public:
    virtual void handler_received_data(void * data) = 0;
};

#endif //SERVERCLIENT_HANDLER_H
