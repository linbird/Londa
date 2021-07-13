#include <memory>
#include <thread>
#include <iostream>
#include <fmt/ostream.h>
#include <fmt/core.h>
#include <chrono>

using namespace std::chrono_literals;

void sub_thread(std::shared_ptr<int> sp){
    std::this_thread::sleep_for(100ms);
    int i = 1;
    while(i--){
        std::cout << fmt::format("5:子线程{}引用计数={}, value={}", std::this_thread::get_id(), sp.use_count(), *sp) << std::endl;
        std::this_thread::sleep_for(10ms);
    }
}

int main(){
    std::thread tmp;
    {
        std::shared_ptr<int> sp(new int(3));
        std::cout << fmt::format("1:主线程{}引用计数={}, value={}", std::this_thread::get_id(), sp.use_count(), *sp) << std::endl;
        auto sp1 = sp;
        std::cout << fmt::format("2:主线程{}引用计数={}, value={}", std::this_thread::get_id(), sp.use_count(), *sp) << std::endl;
        sub_thread(sp1);
        std::cout << fmt::format("3:主线程{}引用计数={}, value={}", std::this_thread::get_id(), sp.use_count(), *sp) << std::endl;
        tmp = move(std::thread(sub_thread, sp));
        std::cout << fmt::format("4:主线程{}引用计数={}, value={}", std::this_thread::get_id(), sp.use_count(), *sp) << std::endl;
    }
    tmp.detach();
    //for(int i = 0; i < std::thread::hardware_concurrency()-1; ++i){
    //    std::thread(sub_thread, sp).detach();
    //}
    //    auto start = std::chrono::steady_clock::now();
    //    while(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start) < 2s){
    //        std::cout << fmt::format("主线程引用计数count = {}", sp.use_count()) << std::endl;
    //        std::this_thread::sleep_for(20ms);
    //    }
    std::this_thread::sleep_for(2s);
    return 0;
}
/*
1:主线程140005779740480引用计数=1, value=3
2:主线程140005779740480引用计数=2, value=3
5:子线程140005779740480引用计数=3, value=3
3:主线程140005779740480引用计数=2, value=3
4:主线程140005779740480引用计数=3, value=3
5:子线程140005779736320引用计数=1, value=3
*/
