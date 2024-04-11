#ifndef SERVERCLIENT_CIRCULAR_BUFFER_H
#define SERVERCLIENT_CIRCULAR_BUFFER_H


template <typename T, int BUFF_SZ>
struct circular_buffer {
private:
    T buff[BUFF_SZ];

    int base_in = 0;
    int count = 0;

    // the current buffer is [base_in, base_in + count), (circular indexes)

public:

    inline bool empty() {
        return count == 0;
    }

    inline bool full() {
        return count == BUFF_SZ;
    }


    inline T& front() {
        return buff[base_in];
    }

    inline T& rear() {
        return buff[(base_in + count - 1) % BUFF_SZ];
    }


    inline bool pop_front() {
        if (empty()) return false;

        base_in = (base_in + 1) % BUFF_SZ;
        count--;
        return true;
    }

    inline bool pop_back() {
        if (empty()) return false;

        count--;
        return true;
    }

    inline bool push_back(T val) {
        if (full()) return false;

        buff[(base_in + count) % BUFF_SZ] = std::move(val);
        count++;
        return true;
    }

    inline bool push_front(T val) {
        if (full()) return false;

        base_in = (base_in - 1 + BUFF_SZ) % BUFF_SZ;
        buff[base_in] = std::move(val);
        count++;
        return true;
    }

};


#endif //SERVERCLIENT_CIRCULAR_BUFFER_H
