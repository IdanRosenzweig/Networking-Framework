#ifndef SERVERCLIENT_CIRCULAR_BUFFER_H
#define SERVERCLIENT_CIRCULAR_BUFFER_H

#include <utility>
#include <thread>
#include <thread>

// simple circular queue ds
template <typename T, int BUFF_SZ = 1024>
struct circular_buffer {
private:
    T buff[BUFF_SZ];

    int base_in = 0;
    int count = 0;

    // the current buffer is [base_in, base_in + count), (circular indexes)

public:

    inline bool is_empty() {
        return count == 0;
    }

    inline bool is_full() {
        return count == BUFF_SZ;
    }

    template <int MAX_MILLISEC_TIMEOUT = 1000>
    inline T* front() {
        if (!is_empty()) return &buff[base_in];

        const auto start = std::chrono::high_resolution_clock::now();
        while (is_empty()) {
            const auto curr = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start);

            if (duration > std::chrono::milliseconds(MAX_MILLISEC_TIMEOUT)) return nullptr;
        }

        return &buff[base_in];
    }

    template <int MAX_MILLISEC_TIMEOUT = 1000>
    inline T* rear() {
        if (!is_empty()) return &buff[(base_in + count - 1) % BUFF_SZ];

        const auto start = std::chrono::high_resolution_clock::now();
        while (is_empty()) {
            const auto curr = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curr - start);

            if (duration > std::chrono::milliseconds(MAX_MILLISEC_TIMEOUT)) return nullptr;
        }

        return &buff[(base_in + count - 1) % BUFF_SZ];
    }


    inline bool pop_front() {
        if (is_empty()) return false;

        base_in = (base_in + 1) % BUFF_SZ;
        count--;
        return true;
    }

    inline bool pop_back() {
        if (is_empty()) return false;

        count--;
        return true;
    }

    inline void clear() {
        count = 0;
    }

    inline bool push_back(T val) {
        if (is_full()) return false;

        buff[(base_in + count) % BUFF_SZ] = std::move(val);
        count++;
        return true;
    }

    inline bool push_front(T val) {
        if (is_full()) return false;

        base_in = (base_in - 1 + BUFF_SZ) % BUFF_SZ;
        buff[base_in] = std::move(val);
        count++;
        return true;
    }

};


#endif //SERVERCLIENT_CIRCULAR_BUFFER_H
