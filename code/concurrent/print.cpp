#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

mutex print;
condition_variable cv;
atomic<bool> odd(true);
atomic<bool> odd_end(false);

const int MAX = 100;

void print_even(){
    int start = 1;
    while(start < MAX){
        unique_lock<mutex> locker(print, adopt_lock);
        cv.wait(locker, [](){return odd.load();});
        cout << start << ' ' << flush;
        if(start+2 < MAX) cout << start+2 << ' ' << flush;
        start += 4;
        odd.store(false);
        cv.notify_one();
//：this_thread::sleep_for(1ms);
    }
    odd_end.store(true);
    cv.notify_all();
}

void print_odd(){
    int start = 2;
    while(start <= MAX){
        unique_lock<mutex> locker(print);
        cv.wait(locker, [](){return !odd.load();});
        cout << start << ' ' << flush;
        start += 2;
        if(!odd_end.load()) odd.store(true);
        cv.notify_one();
    }
}

int main(){
    print.lock();
    thread(print_odd).detach();
    thread(print_even).join();
    return 0;
}
