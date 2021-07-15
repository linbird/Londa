#include <iostream>
#include <chrono>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <thread>

using namespace std::chrono_literals;

//实际创建的线程的数量可以大于硬件最大并行数量
int main(){
    for(int i = 0; i < 20; ++i){
        std::thread([i]{
            std::cout << fmt::format("启动线程{}，开始休眠等待10s\n", i) << std::endl;
            std::this_thread::sleep_for(10s);
            std::cout << fmt::format("\t结束线程{}\n", i) << std::endl;
        }).detach();
    }
    std::this_thread::sleep_for(20s);
    return 0;
}
