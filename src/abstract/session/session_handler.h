#ifndef NETWORKING_SESSION_HANDLER_H
#define NETWORKING_SESSION_HANDLER_H

#include <memory>

// abstract class that handles a new opened session.
// the pointer to the session will be given by the instantiator of the object.
// just implement the class
template<typename SESSION_TYPE>
class session_handler {
public:
    SESSION_TYPE session; // pointer to the session itself. this would be provided by the session manager upon creating the handler

    session_handler() {}
    session_handler(SESSION_TYPE &&sess) : session(std::move(sess)) {
    }

    virtual ~session_handler() = default;

    virtual void on_session_start() = 0;

};

#endif //NETWORKING_SESSION_HANDLER_H
