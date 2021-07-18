#include <algorithm>
#include <array>
#include <sstream>
#include <thread>
#include <iterator>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <list>
#include <iostream>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>
#include <fmt/format.h>
#include <random>
#include <chrono>
#include <atomic>

using std::cout;
using std::endl;
using std::list;
using std::thread;
using namespace std::chrono;
using namespace std::chrono_literals;

list<float> data_pool;
std::mutex protecter;
std::condition_variable cv;

std::random_device rd;
std::minstd_rand generator(rd());
std::normal_distribution<float> distributer(5.0, 4.7);

std::atomic<bool> end_flag(false);

std::string current_time(){
    timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", std::gmtime(&ts.tv_sec));
    std::stringstream ssr;
    ssr << buff << '.' << ts.tv_nsec;
    return ssr.str();
}

void producer(milliseconds limit){
    fmt::print(fmt::fg(fmt::color::yellow), fmt::format("{}：生产者({}) 开始运行\n", current_time(), std::this_thread::get_id()));
    auto start = steady_clock::now();
    while(duration_cast<milliseconds>(steady_clock::now() - start) < limit){
        std::vector<float> data_stage(8);
        std::generate(data_stage.begin(), data_stage.end(), [](){return distributer(generator);}) ;
        {
            std::lock_guard<std::mutex> locker(protecter);
            copy(data_stage.begin(), data_stage.end(), back_inserter(data_pool));
            fmt::print(fmt::fg(fmt::color::blue), fmt::format("{}：生产者({}) 生成并向数据池注入{}条数据\n", current_time(), std::this_thread::get_id(), 8));

            if(data_pool.size() >= 100){
                fmt::print(fmt::fg(fmt::color::red), fmt::format("{}：生产者({}) 检测到数据池有{}条数据，唤醒消费者\n", current_time(), std::this_thread::get_id(), data_pool.size()));
                cv.notify_one();
            }
        }
    }
    end_flag.store(true);
    cv.notify_one();
    fmt::print(fmt::bg(fmt::color::red), fmt::format("\n{}：生产者({}) 运行{}ms后结束\n", current_time(), std::this_thread::get_id(), duration_cast<milliseconds>(steady_clock::now() - start).count()));
}

void consumer(){
    fmt::print(fmt::fg(fmt::color::yellow), fmt::format("{}：消费者({}) 开始运行\n", current_time(), std::this_thread::get_id()));
    thread_local int batch_size = 100;
    //while (!end_flag.load()) {
    while (1) {

        std::unique_lock<std::mutex> locker(protecter);
        cv.wait(locker);
        if(end_flag.load()){
            fmt::print(fmt::bg(fmt::color::green) | fmt::fg(fmt::color::blue), fmt::format("{}：消费者({}) 收到结束请求\n", current_time(), std::this_thread::get_id()));
            break;
        }
        fmt::print(fmt::fg(fmt::color::green), fmt::format("{}：消费者({}) 被唤醒，数据池有{}条数据，将取走{}条数据\n", current_time(), std::this_thread::get_id(), data_pool.size(), batch_size));
        auto begin = data_pool.begin(), end = next(data_pool.begin(), batch_size);
        //        std::copy(make_move_iterator(begin), make_move_iterator(end), std::ostream_iterator<float>(std::cout, " "));
        data_pool.erase(begin, end);
        fmt::print(fmt::fg(fmt::color::green), fmt::format("{}：消费者({}) 消费后数据池还有{}条数据\n", current_time(), std::this_thread::get_id(), data_pool.size(), batch_size));
    }
    if(data_pool.size()){
        fmt::print(fmt::bg(fmt::color::green) | fmt::fg(fmt::color::blue), fmt::format("{}：消费者({}) 数据池中剩余{}数据, 将一次读取完\n", current_time(), std::this_thread::get_id(), data_pool.size()));
        batch_size = data_pool.size();
        data_pool.erase(data_pool.begin(), data_pool.end());
    }
    fmt::print(fmt::bg(fmt::color::green), fmt::format("{}：消费者({}) 结束，数据池中剩余{}数据\n", current_time(), std::this_thread::get_id(), data_pool.size()));
}

int main(){
    //    std::thread(consumer).join();
    //    std::thread(producer, 2s).join();
    std::array<thread, 2> threads{thread(producer, 3ms), thread(consumer)};
    for(auto& _thread : threads)
        _thread.join();
    cout << endl;
    return 0;
}
