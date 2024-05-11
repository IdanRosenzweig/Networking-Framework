#ifndef SERVERCLIENT_BASIC_SESSION_GENERATOR_H
#define SERVERCLIENT_BASIC_SESSION_GENERATOR_H

#include "basic_session.h"
#include "../receiving/multi_receiver.h"
#include <memory>
#include <type_traits>

// object that generates new sessions

template <typename T = basic_session>
class basic_session_generator : public multi_receiver<std::unique_ptr<T>> {
    static_assert(std::is_base_of_v<basic_session, T>, "template for basic_session_generator must be derived type of basic_session");
};

#endif //SERVERCLIENT_BASIC_SESSION_GENERATOR_H
