//1.写一段简单的程序，启动两个线程交替打印1–100

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using std::cout;
using std::endl;
using std::thread;
using std::vector;
using std::mutex;

#define DEBUG
//#undef DEBUG

mutex m;

void print_even(){
    thread_local static int num = 0;
#ifdef DEBUG
    cout << endl << "偶数输出线程" << std::this_thread::get_id() << ": ";
#endif
    while(num != 102){
        m.try_lock();
        cout << num << ' ';
        m.unlock();
        num += 2;
    }
}

void print_odd(){
    thread_local static int num = 1;
#ifdef DEBUG
    cout << endl << "奇数输出线程：" << std::this_thread::get_id() << ": ";
#endif
    while(num != 101){
        m.try_lock();
        cout << num << ' ';
        m.unlock();
        num += 2;
    }
}

int main(){
    cout << "启动两个线程交替打印1–100" << endl;
    //    vector<thread> t{print_even, print_odd};
    //    t[0].detach();
    //    t[1].join();
    thread(print_even).detach();
    thread(print_odd).join();
    return 0;
}
