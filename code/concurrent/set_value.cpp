#include <iostream>
#include <future>
#include <thread>

int main()
{
    using namespace std::chrono_literals;
    std::promise<int> p;
    std::future<int> f = p.get_future();
    std::thread([&p] {
            std::this_thread::sleep_for(1s);
            p.set_value_at_thread_exit(9);
            }).detach();

    auto func = [](std::promise<int> promise){
        promise.set_value_at_thread_exit(11);///z只在线程中起作用
    };
    std::promise<int> test;
    std::future<int> tmp = test.get_future();
    func(std::move(test));

    std::this_thread::sleep_for(100ms);
    std::cout << "Waiting" << std::endl;
    std::cout << tmp.get() << std::endl;

    std::cout << "Waiting..." << std::flush;
    f.wait();
    std::cout << "Done!\nResult is: " << f.get() << '\n';
    return 0;
}
