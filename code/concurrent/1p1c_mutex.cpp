#include <bits/types/struct_timespec.h>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <string>
#include <atomic>

using namespace std::chrono_literals;
using namespace std;

random_device rd;
mt19937 generator(rd());
uniform_int_distribution<int> distributer(1, 20);

vector<int> data_buffer;
mutex protecter;
atomic<bool> end_flag(false);

string current(){
    auto x = chrono::system_clock::now();
    auto y = x.time_since_epoch();
    auto ms_ = chrono::duration_cast<chrono::milliseconds>(y) - chrono::duration_cast<chrono::seconds>(y);
    time_t z = chrono::system_clock::to_time_t(x);
    string date(ctime(&z));
    date.erase(date.end()-1);
    date.push_back('.');
    date.append(to_string(ms_.count()));
    return date;
}

void producer(){
    fmt::print(fmt::bg(fmt::color::brown), fmt::format("生产者({} @ {})：开始运行\n", this_thread::get_id(), current()));
    auto start_tp = chrono::steady_clock::now();
    auto current_time = start_tp;
    do{
        {
            lock_guard<mutex> locker(protecter);
            fmt::print(fmt::fg(fmt::color::red), fmt::format("生产者({} @ {})：获取锁\n", this_thread::get_id(), current()));
            data_buffer.push_back(distributer(generator));
        }
        fmt::print(fmt::fg(fmt::color::green), fmt::format("生产者({} @ {})：释放锁\n", this_thread::get_id(), current()));
        this_thread::sleep_for(5ms);
        current_time = chrono::steady_clock::now();
    }
    while(chrono::duration_cast<chrono::milliseconds>(current_time - start_tp) < 100ms);
    end_flag.store(true);
    fmt::print(fmt::bg(fmt::color::brown), fmt::format("生产者({} @ {})：结束运行\n", this_thread::get_id(), current()));
}

void consumer(){
    fmt::print(fmt::bg(fmt::color::brown), fmt::format("\t消费者({} @ {})：开始运行\n", this_thread::get_id(), current()));
    while(!end_flag.load()){
        {
            lock_guard<mutex> locker(protecter);
            fmt::print(fmt::fg(fmt::color::red), fmt::format("\t消费者({} @ {})：获取锁\n", this_thread::get_id(), current()));
            if(data_buffer.size() != 0){
                fmt::print(fmt::fg(fmt::color::blue), fmt::format("\t消费者({} @ {})：取出数据{} \n", this_thread::get_id(), current(), data_buffer.back()));
                data_buffer.erase(data_buffer.end() - 1);
            }else{
                fmt::print(fmt::fg(fmt::color::blue), fmt::format("\t消费者({} @ {})：无数据 \n", this_thread::get_id(), current()));
            }
        }
        fmt::print(fmt::fg(fmt::color::green), fmt::format("\t消费者({} @ {})：释放锁\n", this_thread::get_id(), current()));
        this_thread::sleep_for(5ms);
    }
    fmt::print(fmt::bg(fmt::color::brown), fmt::format("\t消费者({} @ {})：结束运行\n", this_thread::get_id(), current()));
}

int main(){
    thread(producer).detach();
    //    thread(producer).join();
    thread(consumer).join();
    return 0;
}
