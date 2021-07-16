#include <atomic>
#include <ostream>
#include <thread>
#include <exception>
#include <iostream>
#include <future>
#include <chrono>

using namespace std;

using std::thread;
using namespace std::chrono_literals;

atomic<bool> end_flag(false);

void Myfunction(std::promise<int>& res){
//    res.set_value_at_thread_exit(1100);
    std::this_thread::sleep_for(1000ms);
    res.set_value(1100);
    return ;
}

void print_process(){
    while(1){
        std::cout << "*" << std::flush;
        std::this_thread::sleep_for(10ms);
        if(end_flag.load()){
            cout << endl;
            break;
        }
    }
}

int main(){
    std::promise<int> courier;
    std::future<int> res = courier.get_future();
    std::thread(Myfunction, std::ref(courier)).detach();
    thread(print_process).detach();
    int res_value = 0;
    while(1){
        try{
            //std::this_thread::sleep_for(2000ms);
            res_value = res.get();//阻塞了
            end_flag.store(true);
            res.get();
            break;
        }catch(exception e){
            cout << "触发异常" << e.what() << endl;
            this_thread::sleep_for(20ms);
            continue;
        }
    }
    cout << res_value << endl;
    return 0;
}
