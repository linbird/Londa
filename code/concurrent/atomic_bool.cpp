#include <condition_variable>
#include <exception>
#include <mutex>
#include <thread>
#include <chrono>
#include <iostream>
#include <future>

using namespace std::chrono_literals;
using namespace std;

atomic<bool> end_flag(false);

int main(){
    auto func = [](promise<int>& promise, chrono::milliseconds limit, int res){//将第二个参数设置成时间参数
        promise.set_value_at_thread_exit(res);
        auto start = chrono::steady_clock::now();
        while(chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start) < limit){
            if(end_flag.load()){
                cout << "主线程要求本线程结束" << endl;
                return;
            }
            cout << "*" << flush;
            this_thread::sleep_for(10ms);
        }
    };

    promise<int> promise;
    future<int> res = promise.get_future();
    thread(func, ref(promise), 100ms, 1).detach();
    this_thread::sleep_for(70ms);
    end_flag.store(true);
    this_thread::sleep_for(10ms);
    cout << res.get() << endl;

    end_flag.store(false);

    std::promise<int> promise1;
    res = promise1.get_future();
    thread(func, ref(promise1), 100ms, 2).detach();
    this_thread::sleep_for(300ms);
    cout << res.get() << endl;

    return 0;
}
