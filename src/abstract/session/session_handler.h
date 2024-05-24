#ifndef SERVERCLIENT_SESSION_HANDLER_H
#define SERVERCLIENT_SESSION_HANDLER_H

#include <memory>

// abstract class that handles a new opened session.
// the reference to the session will be given by the instantiator of the object.
// just implement the class

template<typename SESSION_TYPE>
class session_handler {
protected:
    SESSION_TYPE& session; // reference to the session itself. this would be provided by the session manager upon creating a app_handler

public:

    explicit session_handler(SESSION_TYPE &sess) : session(sess) {
    }

    virtual ~session_handler() {}

    virtual void on_session_start() = 0;

};

#endif //SERVERCLIENT_SESSION_HANDLER_H
