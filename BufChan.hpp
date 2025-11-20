#include <bits/stdc++.h>
using namespace std;

/*
when channel is closed, receiver must still be able to receive items inside the channel.

for closing, suppose buffer size is 1, and have 99 senders that are blocking, it is possible
that the closer thread inserts the ending val 1st.

guarantee that if send(T t) returns true, a receiver can eventually receive t, 
if there are enough receivers.

1 insight is that there can only be blocked senders or blocked receivers, never both.
if send() notify one, it is always a blocked receiver.
if recv() notify one, it is always a blocked sender.
*/

template <typename T>
concept ValueInitializable = requires { T(); };

struct Ctrl {
    bool closed;
    int len;
};

template <ValueInitializable T>
struct BufChan {
    bool closed = false;
    int s = 0, e = 0, curr_size = 0;
    vector<T> buf;
    mutex mut{};
    condition_variable cv;

    BufChan(int len): buf(len, T()) {}

    void close() {
        mut.lock();
        closed = true;
        mut.unlock();
        cv.notify_all();
    }

    pair<T,bool> recv() {
        pair<T,bool> out;
        {
            unique_lock lk{mut};
            if (closed) return {T(), false};
            while (!closed && curr_size == 0) cv.wait(lk);
            if (closed) return {T(), false};
            curr_size--;
            out = {buf[s], true};
            s = (s + 1) % buf.size();
        }
        cv.notify_one();
        return out;
    }

    bool send(T t) {
        {
            unique_lock lk{mut};
            if (closed) return false;
            while (!closed && curr_size == buf.size()) cv.wait(lk);
            if (closed) return false;
            curr_size++;
            buf[e] = t;
            e = (e + 1) % buf.size();
        }
        cv.notify_one();
        return true;
    }

};