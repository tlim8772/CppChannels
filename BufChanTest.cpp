#include "BufChan.hpp"

void testSend1() {
    BufChan<int> chan{5};

    // store the results, here, so we can print once all the threads finish.
    // to avoid output inteleaving.
    vector<pair<int,bool>> send_res, recv_res;

    auto t1 = [&chan, &send_res] () {
        //this_thread::sleep_for(300ms);
        for (int i = 0; i < 8000; i++) {
            bool res = chan.send(i);
            if (res) send_res.emplace_back(i, res);
        }
    };

    auto t2 = [&chan, &recv_res] () {
        for (int i = 0; i < 9000; i++) {
            auto [v, res] = chan.recv();
            if (res) recv_res.emplace_back(v, res);
        }
    };

    auto t3 = [&chan] () {
        this_thread::sleep_for(10ms);
        chan.close();
    };

    {
        vector<jthread> ths;
        ths.emplace_back(t1);
        ths.emplace_back(t2);
        ths.emplace_back(t3);
    }

    for (auto p : send_res) {
        cout << "send " << p.first << " " << p.second << endl; 
    }
    cout << endl;

    for (auto p : recv_res) {
        cout << "recv " << p.first << " " << p.second << endl;
    }

    cout << endl;
}

int main() {
    testSend1();
}