#include <chrono>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std::chrono_literals;

using std::cout;
using std::endl;
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::thread;

int num = 0;
mutex num_mutex;
condition_variable cv;

void print(int id){
    static int max_p = thread::hardware_concurrency()-1;
    while(num <= 100){
        {
        unique_lock<mutex> locker(num_mutex);
        cv.wait(locker, [id](){return num%max_p == id;});///此语句主要是为了让每个线程交替输出
        cout << id << '(' << std::this_thread::get_id() << ") " << num << endl;
        ++num;
        cv.notify_all();
        }
    }
}

int main(){
    for(int i = 0; i < thread::hardware_concurrency()-1; ++i)
        thread(print, i).detach();
//    std::this_thread::sleep_for(100ms);///等待所有子线程执行完毕
    pthread_exit(NULL);///等待所有子线程执行完毕
    return 0;
}
