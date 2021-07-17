#include <iostream>
#include <future>
#include <chrono>
#include <thread>

int main(){
    std::promise<int> promise;
    std::future<int> res = promise.get_future();

    std::thread test_thread([](std::promise<int> promise){
        promise.set_value_at_thread_exit(12);
    }, std::move(promise));

    test_thread.detach();

    std::cout << res.valid() << std::endl;

    std::shared_future<int> sh_res = res.share();///此操作将导致future无效
    std::cout << res.get() << std::endl;
    for(int i = 0; i < 12; ++i){
        std::cout << i << ':' << sh_res.get() << ':' << res.valid() << std::endl;
    }

    return 0;
}
