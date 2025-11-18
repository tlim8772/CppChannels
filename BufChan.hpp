#include <bits/stdc++.h>
using namespace std;

/*
when channel is closed, receiver must still be able to receive items inside the channel.

for closing, suppose buffer size is 1, and have 99 senders that are blocking, it is possible
that the closer thread inserts the ending val 1st.

want to guarantee that if send(T t) returns true, a receiver can eventually receive t.
*/

template <typename T>
concept ValueInitializable = requires { T(); };

struct Ctrl {
    bool closed;
    int len;
};

template <ValueInitializable T>
struct BufChan {
    const int buf_size;
    atomic<Ctrl> ctrl;
    atomic<int> s{0}, e{0};
    vector<T> buf;

    BufChan(int len): buf_size(len), ctrl{Ctrl{false, 0}}, buf(len, T()) {}

    void close() {
        Ctrl c = ctrl.load();
        while (!ctrl.compare_exchange_strong(c, {true, c.len}));
    }

    pair<T, bool> recv() {
        Ctrl c;
        start:
            while (1) {
                c = ctrl.load();
                if (c.len == 0 && c.closed) return {T(), false};
                if (c.len > 0) break;
            }

        while (!ctrl.compare_exchange_strong(c, {c.closed, c.len - 1})) {
            if (c.len == 0 && c.closed) return {T(), false};
            else if (c.len == 0) goto start;
        }

        int p = s.load();
        while (!s.compare_exchange_strong(p, (p + 1) % buf_size));
        return {buf[p], true};
    }

    bool send(T t) {
        Ctrl c;
        start:
            while (1) {
                c = ctrl.load();
                if (c.closed) return false;
                if (c.len < buf_size) break;
            }

        while (!ctrl.compare_exchange_strong(c, {c.closed, c.len + 1})) {
            if (c.closed) return false;
            if (c.len == buf_size) goto start;
        }

        int p = e.load();
        while (!e.compare_exchange_strong(p, (p + 1) % buf_size));
        buf[p] = std::move(t);

        return true;
    }


};