#include <atomic>
#include <iostream>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

using namespace std::chrono_literals;

std::atomic<int> times(0);

void once_function(){
    std::cout << fmt::format("线程{}第{}次执行次函数\n", std::this_thread::get_id(), ++times);
}

int main(){
    for(int i  = 0; i < std::thread::hardware_concurrency()-1; ++i)
        std::thread(once_function).detach();
    std::this_thread::sleep_for(100ms);

    std::once_flag flag;//被call_once和once_flag作用的函数才会被限制调用
    std::call_once(flag, once_function);
    std::call_once(flag, once_function);
    std::call_once(flag, once_function);

    for(int i  = 0; i < std::thread::hardware_concurrency()-1; ++i)
        std::thread(once_function).detach();
    std::this_thread::sleep_for(100ms);
    return 0;
}
