#ifndef SERVERCLIENT_RING_BUFFER_H
#define SERVERCLIENT_RING_BUFFER_H


template <typename T, int MAX_NO_MSG>
struct ring_buffer {
private:
    static constexpr int BUFF_SZ = MAX_NO_MSG + 1;

    T ring_buff[BUFF_SZ];

    int front_in = 0;
    int back_in = 0;

    // the current buffer is [front_in, back_in), (circular indexes)

public:

    inline bool empty() {
        return front_in == back_in;
    }

    inline bool full() {
        return (back_in + 1) % BUFF_SZ == front_in;
    }


    inline T& front() {
        return ring_buff[front_in];
    }

    inline T& rear() {
        return ring_buff[(back_in - 1 + BUFF_SZ) % BUFF_SZ];
    }


    inline bool pop() {
        if (empty()) return false;

        front_in = (front_in + 1) % BUFF_SZ;
        return true;
    }

    inline bool push(T&& val) {
        if (full()) return false;

        ring_buff[back_in] = std::move(val);
        back_in = (back_in + 1) % BUFF_SZ;
        return true;
    }

};

#endif //SERVERCLIENT_RING_BUFFER_H
