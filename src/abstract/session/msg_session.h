#ifndef SERVERCLIENT_MSG_SESSION_H
#define SERVERCLIENT_MSG_SESSION_H

#include "../sending/msg/send_msg.h"
#include "../receiving/msg/received_msg.h"
#include "basic_session.h"
#include "basic_session_generator.h"
#include "basic_session_handler.h"
#include "basic_sessions_manager.h"

using msg_session = basic_session<send_msg, received_msg>;
using msg_session_generator = basic_session_generator<msg_session>;
using msg_session_handler = basic_session_handler<msg_session>;
using msg_session_manager = basic_sessions_manager<msg_session, msg_session_handler>;

#endif //SERVERCLIENT_MSG_SESSION_H
