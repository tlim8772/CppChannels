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

void testSend2() {
    BufChan<int> chan{5};
    vector<pair<int,bool>> send1, send2, recv1;

    auto s1 = [&send1, &chan] () {
        for (int i = 0; i < 100; i++) {
            bool res = chan.send(i);
            if (res) send1.emplace_back(i, res);
        }
    };

    auto s2 = [&send2, &chan] () {
        for (int i = 100; i < 200; i++) {
            bool res = chan.send(i);
            if (res) send2.emplace_back(i, res);
        }
    };

    auto r1 = [&recv1, &chan] () {
        for (int i = 0; i < 200; i++) {
            auto [v, res] = chan.recv();
            if (res) recv1.emplace_back(v, res);
        }
    };

    {
        vector<jthread> ths;
        ths.emplace_back(s1);
        ths.emplace_back(s2);
        ths.emplace_back(r1);
    }

    for (auto [v, r] : send1) {
        cout << "send " << v << " " << r << endl;
    }
    cout << endl;
    for (auto [v, r] : send2) {
        cout << "send " << v << " " << r << endl;
    }
    cout << endl;

    sort(recv1.begin(), recv1.end(), [](pair<int,bool> p1, pair<int,bool> p2) { return p1.first < p2.first; });
    for (auto [v, r] : recv1) {
        cout << "recv " << v << " " << r << endl;
    };
}

void testSend3() {
    BufChan<int> chan{5};
    vector<pair<int,bool>> send1, send2, recv1, recv2;

    auto s1 = [&send1, &chan] () {
        for (int i = 0; i < 100; i++) {
            bool res = chan.send(i);
            if (res) send1.emplace_back(i, res);
        }
    };

    auto s2 = [&send2, &chan] () {
        for (int i = 100; i < 200; i++) {
            bool res = chan.send(i);
            if (res) send2.emplace_back(i, res);
        }
    };

    auto r1 = [&recv1, &chan] () {
        for (int i = 0; i < 100; i++) {
            auto [v, res] = chan.recv();
            if (res) recv1.emplace_back(v, res);
        }
    };

     auto r2 = [&recv2, &chan] () {
        for (int i = 0; i < 100; i++) {
            auto [v, res] = chan.recv();
            if (res) recv2.emplace_back(v, res);
        }
    };



    {
        vector<jthread> ths;
        ths.emplace_back(s1);
        ths.emplace_back(s2);
        ths.emplace_back(r1);
        ths.emplace_back(r2);
    }

    for (auto [v, r] : send1) {
        cout << "send " << v << " " << r << endl;
    }
    cout << endl;
    for (auto [v, r] : send2) {
        cout << "send " << v << " " << r << endl;
    }
    cout << endl;

    sort(recv1.begin(), recv1.end(), [](pair<int,bool> p1, pair<int,bool> p2) { return p1.first < p2.first; });
    for (auto [v, r] : recv1) {
        cout << "recv " << v << " " << r << endl;
    };
    cout << endl;

    sort(recv2.begin(), recv2.end(), [](pair<int,bool> p1, pair<int,bool> p2) { return p1.first < p2.first; });
    for (auto [v, r] : recv2) {
        cout << "recv " << v << " " << r << endl;
    };
    cout << endl;
}

void testBuffer() {
    BufChan<int> chan{5};

    auto t1 = [&chan] () {
        for (int i = 0; i < 5; i++) {
            bool res = chan.send(i);
            cout << "send " << i << " " << res << endl;
        }
    };

    {
        jthread{t1};
    }

}

int main() {
    //testSend1();
    //testSend2();
    //testSend3();
    testBuffer();
}