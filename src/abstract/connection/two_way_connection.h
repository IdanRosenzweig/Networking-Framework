// #pragma once

// #include "../sending/basic_send_medium.h"
// #include "../receiving/basic_recv_listener.h"

// #include <memory>
// using namespace std;

// // a basic two-way connection that you can send data on and listen to data from
// template <typename TYPE_SEND, typename TYPE_RECV>
// struct two_way_connection {
// public:
//     shared_ptr<basic_send_medium<TYPE_SEND>> send;
//     shared_ptr<basic_recv_listener<TYPE_RECV> > recv;
// };