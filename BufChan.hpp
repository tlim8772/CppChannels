#include <bits/stdc++.h>
using namespace std;

/*
when channel is closed, receiver must still be able to receive items inside the channel.

guarantee that if send(T t) returns true, a receiver can eventually receive t, 
if there are enough receivers.

1 insight is that there can only be blocked senders or blocked receivers, never both.
below is only true if channel is not closed.
if send() notify one, it is always a blocked receiver.
if recv() notify one, it is always a blocked sender.

close() needs to notify one only, because the chosen sender/receiver who wakes up will also 
call notify one, which will wake up the next blocked sender/receiver.
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
            while (!closed && curr_size == 0) cv.wait(lk);
            
            // if curr_size > 0, there is still an elem we can take
            // anyway, if take the if branch, no notify will be called.
            // hence, close() must call notify_all to wake up all threads sleeping on cv
            if (closed && curr_size == 0) return {T(), false}; 
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