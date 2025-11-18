#include "UnBufChan.hpp"

// test the 'barrier' prperty of UnBufChan
void testBarrier() {
    array<int,2> store = {0, 0};
    UnBufChan<int> chan;

    auto t1 = [&store, &chan] () {
        for (int i = 0; i < 1000000; i++) {
            int idx = (0 + i) % 2;
            store[idx]++;
            chan.send(1);
        }
    };

    auto t2 = [&store, &chan] () {
        for (int i = 0; i < 1000000; i++) {
            int idx = (1 + i) % 2;
            store[idx]++;
            chan.recv();
        }
    };

    {
        vector<jthread> ths;
        ths.emplace_back(t1);
        ths.emplace_back(t2);
    }

    cout << store[0] << " " << store[1] << endl;
    cout << endl;
}

void testSend() {
    UnBufChan<string> chan;

    auto t1 = [&chan] () {
        string s = "hello world";
        this_thread::sleep_for(300ms);
        cout << "sending " << s << endl;
        chan.send(s);
    };

    auto t2 = [&chan] () {
        auto [v, res] = chan.recv();
        cout << "received " << v << endl;
    };

    {
        vector<jthread> ths;
        ths.emplace_back(t1);
        ths.emplace_back(t2);
    }

    cout << endl;
}

void testSend2() {
    UnBufChan<int> chan;

    auto t1 = [&chan] (int x) {
        for (int i = 0; i < x; i++) {
            chan.send(i);
        }
    };

    auto t2 = [&chan] (int x) {
        for (int i = 0; i < x; i++) {
            auto [v, res] = chan.recv();
            cout << v << endl;
        }
    };

    {
        vector<jthread> ths;
        ths.emplace_back(t1, 4);
        ths.emplace_back(t2, 2), ths.emplace_back(t2, 2);
    }

    cout << endl;
}

void testSend3() {
    UnBufChan<int> chan;

    auto t1 = [&chan] (int x) {
        for (int i = 0; i < x; i++) {
            chan.send(i);
        }
        chan.close();
    };

    auto t2 = [&chan] (int x) {
        for (int i = 0; i < x; i++) {
            auto [v, res] = chan.recv();
            cout << v << " " << res << endl;
        }
    };

    {
        vector<jthread> ths;
        ths.emplace_back(t1, 10);
        ths.emplace_back(t2, 6), ths.emplace_back(t2, 7);
    }

    cout << endl;
}


int main() {
    testBarrier();
    testSend();
    testSend2();
    testSend3();
}