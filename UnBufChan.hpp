#include <bits/stdc++.h>
#include <semaphore>
using namespace std;

template <typename T>
concept ValueInitializable = requires { T(); };


template <ValueInitializable T>
struct UnBufChan {
    atomic_flag closed{};
    T t;
    counting_semaphore<1000000> send_sem{1}, recv_sem{0};

    UnBufChan() {}

    bool send(T val) {
        if (closed.test()) return false;
        
        send_sem.acquire();
        if (closed.test()) {
            send_sem.release();
            return false;
        }
        
        t = std::move(val);
        recv_sem.release();
        
        send_sem.acquire();
        send_sem.release();

        return true;
    }

    pair<T, bool> recv() {
        if (closed.test()) return {T(), false};
        
        recv_sem.acquire();
        if (closed.test()) {
            recv_sem.release();
            return {T(), false};
        }

        pair<T, bool> out = {std::move(t), true};
        send_sem.release();
        
        return out;
    }

    // crudely implemented
    void close() {
        send_sem.acquire();
        closed.test_and_set();
        
        send_sem.release();
        recv_sem.release();
    }

};