#ifndef SERVERCLIENT_BASIC_SESSION_HANDLER_H
#define SERVERCLIENT_BASIC_SESSION_HANDLER_H

#include <memory>

// abstract class that handles a new opened session.
// the reference to the session will be given by the instantiator of the object.
// just implement the class

template<typename SESSION_T>
class basic_session_handler {
//    static_assert(std::is_base_of_v<basic_msg_session, SESSION_T>,
//                  "template for basic_session_handler session_type must be derived type of basic_session");

protected:
    std::unique_ptr<SESSION_T> &session; // reference to the session itself. this would be provided by the session manager upon creating a app_handler

public:

    explicit basic_session_handler(std::unique_ptr<SESSION_T> &session) : session(session) {
    }

    virtual ~basic_session_handler() {}

    virtual void session_started() = 0;

};

#endif //SERVERCLIENT_BASIC_SESSION_HANDLER_H
