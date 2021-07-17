#include <ios>
#include <iostream>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <ostream>
#include <string>
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono_literals;

int main(){
    auto after2s = std::chrono::steady_clock::now() + 200ms;
    std::promise<int> promise_2s;
    std::future<int> future_2s = promise_2s.get_future();

    std::thread([](std::promise<int>& promise){
            promise.set_value_at_thread_exit(1);
            std::this_thread::sleep_for(2000ms);
            }, std::ref(promise_2s)).detach();

    auto status = future_2s.wait_until(after2s);
    if(status == std::future_status::timeout){
        std::cout << "wait_until等待超时" << std::endl;
    }else if(status == std::future_status::deferred){
        std::cout << "wait_until等待延期" << std::endl;
    }else{
        std::cout << "wait_until Ready！！！！" << std::endl;
    }

    status = future_2s.wait_for(1900ms);
    if(status == std::future_status::ready){
        std::cout << "wait_for Ready！！！！" << std::endl;
        std::cout << future_2s.get() << std::endl;
    }else{
        std::cout << "wait_for UnReady！！！！" << std::endl;
    }

    std::cout << std::endl << std::endl;

    after2s = std::chrono::steady_clock::now() + 20ms;
    auto function = [](std::promise<std::string>& promise) -> int{
//        promise.set_value_at_thread_exit(std::string("完成"));///必须是以线程的方式此语句才起左右
        promise.set_value_at_thread_exit("完成");
        std::this_thread::sleep_for(100ms);
//        promise.set_value("hello");
        return 12;
    };
    std::packaged_task<int(std::promise<std::string>& promise)> task(function);

    std::promise<std::string> promise2;
    std::shared_future<std::string> sh_fu = promise2.get_future();
    std::shared_future<int> sh_res = task.get_future();

    auto sh_status = sh_res.wait_until(after2s);
    if(sh_status == std::future_status::timeout){
        std::cout << "wait_until等待超时" << std::endl;
        task(std::ref(promise2));
    }else if(sh_status == std::future_status::deferred){
        std::cout << "wait_until等待延期" << std::endl;
        //        task(std::move(promise2));///形参类型不匹配，此时要求的形参不是引用类型
        task(std::ref(promise2));
    }else{
        std::cout << "wait_until Ready！！！！" << std::endl;
    }

    std::cout << std::boolalpha << sh_fu.valid() << std::endl;
    while(1){
        sh_status = sh_fu.wait_for(100ms);
        if(sh_status == std::future_status::ready){
            std::cout << "wait_for Ready！！！！" << std::endl;
            std::cout << sh_res.get() << std::endl;
            std::cout << sh_fu.get() << std::endl;
            break;
        }else if(sh_status == std::future_status::deferred){
            std::cout << "wait_until等待延期" << std::endl;
        }else{
            std::cout << "wait_for UnReady！！！！" << std::endl;
        }
    }
    return 0;
}
