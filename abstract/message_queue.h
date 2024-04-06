#ifndef SERVERCLIENT_MESSAGE_QUEUE_H
#define SERVERCLIENT_MESSAGE_QUEUE_H

#include "message.h"
#include <queue>

template <int MAX_NO_MSG>
struct message_queue {
//private:
//    message ring_buff[MAX_NO_MSG];
//
//    int front_in = 0;
//    int back_in = 0;
//
//    // the maximum amount of message stored is actually MAX_NO_MSG - 1.
//    // the current buffer is [front_in, back_in), (circular indexes)
//
//public:
//    inline bool empty() {
//        return front_in == back_in;
//    }
//
//    inline bool full() {
//        return (front_in + 1) % MAX_NO_MSG == back_in;
//    }
//
//    inline message& front() {
//        return ring_buff[front_in];
//    }
//
//    inline void pop() {
//        if (!empty())
//            front_in = (front_in + 1) % MAX_NO_MSG;
//    }
//
//    inline void push(message&& msg) {
//        ring_buff[back_in] = std::move(msg);
//        back_in = (back_in + 1) % MAX_NO_MSG;
//    }

    std::queue<message> queue;

};

#endif //SERVERCLIENT_MESSAGE_QUEUE_H
