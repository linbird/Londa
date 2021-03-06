// 读写锁

#include <bits/types/struct_tm.h>
#include <bits/types/time_t.h>
#include <ctime>

#include <exception>
#include <ios>
#include <sstream>
#include <thread>
#include <mutex>
#include <array>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <string>
#include <list>
#include <atomic>
#include <iostream>
#include <locale>
#include <iomanip>

#include <shared_mutex>

using namespace std;
using std::thread;
using namespace std::chrono;
using namespace std::chrono_literals;

list<string> data_list;////数据存储池
shared_mutex data_protecter;
condition_variable_any has_data;///写者写入数据后
condition_variable_any clear_data;///有新的数据

atomic<bool> write_end_flag;
atomic<bool> clear_end_flag;
atomic<int> aready_read(0);
int reader_count = thread::hardware_concurrency() - 2;

mutex new_data_protecter;
condition_variable_any has_new_data;///有新的数据

mutex access_protecter;
int access_record = 0;///访问记录

string timestamp(time_point<system_clock> time_point){
    auto draft = time_point.time_since_epoch();
    time_t current = system_clock::to_time_t(time_point);
    tm current_tm = *localtime(&current);
    string current_str(ctime(&current));
    auto remain = (duration_cast<milliseconds>(draft) - duration_cast<seconds>(draft)).count();
    stringstream stamp_ssr;
    stamp_ssr << put_time(localtime(&current), "%c") << '.' << remain;
    return stamp_ssr.str();
}

void writer(string& path){///从文件写到内存
    fmt::print(fmt::bg(fmt::color::brown), "写者({} @ {})：开始运行\n", this_thread::get_id(), timestamp(system_clock::now()));
    thread_local int warn_threshhold = 70;
    thread_local int critical_threashhold = 90;
    ifstream in_file(path);
    for(string data; getline(in_file, data); ){
        fmt::print(fmt::fg(fmt::color::brown) | fmt::emphasis::bold, "写者({} @ {})：从文件取出数据： {}\n", this_thread::get_id(), timestamp(system_clock::now()), data);
        {
            unique_lock<shared_mutex> locker(data_protecter);
            fmt::print(fmt::fg(fmt::color::red) | fmt::emphasis::bold, "写者({} @ {})：获取独占写锁\n", this_thread::get_id(), timestamp(system_clock::now()));
            int size = data_list.size();
            if(size >= warn_threshhold){
                fmt::print(fmt::bg(fmt::color::pink) | fmt::emphasis::bold, "写者({} @ {})：数据缓冲池为{}达警戒线，将释放独占写锁并停止1ms\n", this_thread::get_id(), timestamp(system_clock::now()), size);
                locker.unlock();
                has_data.notify_all();
                this_thread::sleep_for(1ms);
            }else if(size >= critical_threashhold){
                fmt::print(fmt::bg(fmt::color::red) | fmt::emphasis::bold, "写者({} @ {})：数据缓冲池为{}达危急线，将等待\n", this_thread::get_id(), timestamp(system_clock::now()), size);
                has_data.notify_all();
                fmt::print(fmt::bg(fmt::color::green) | fmt::emphasis::bold, "写者({} @ {})：等待完成\n", this_thread::get_id(), timestamp(system_clock::now()), size);
            }else{}
            data_list.push_back(move(data));
            has_data.notify_all();///通知所有线程有新的数据可以读
            fmt::print(fmt::fg(fmt::color::blue) | fmt::emphasis::bold, "写者({} @ {})：数据缓冲池为{} \n", this_thread::get_id(), timestamp(system_clock::now()), data_list.size());
            this_thread::sleep_for(2ms);
        }
        fmt::print(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "写者({} @ {})：释放独占写锁\n", this_thread::get_id(), timestamp(system_clock::now()));
        data.clear();
    }
    fmt::print(fmt::bg(fmt::color::brown), "写者({} @ {})：结束运行\n", this_thread::get_id(), timestamp(system_clock::now()));
    write_end_flag.store(true);
}

void reader(int id){///thread_local控制每个线程只访问一次
    thread_local bool readed = false;
    string path = "reader_" + to_string(id) + ".copy.cpp";
    ofstream out_file(path);
    fmt::print(fmt::bg(fmt::color::brown), "读者{} ({} @ {})：开始运行\n", id, this_thread::get_id(), timestamp(system_clock::now()));
    while(!clear_end_flag.load()){
        string data;
        {
            shared_lock<shared_mutex> locker(data_protecter);
            fmt::print(fmt::fg(fmt::color::red), "\t读者{} ({} @ {})：获取共享锁\n", id, this_thread::get_id(), timestamp(system_clock::now()));
            has_data.wait(locker);
            data = data_list.front();
        aready_read.fetch_add(1);
        }
        fmt::print(fmt::fg(fmt::color::blue), "\t读者{} ({} @ {})：取出数据 ： {} \n", id, this_thread::get_id(), timestamp(system_clock::now()), data);
        fmt::print(fmt::fg(fmt::color::green), "\t读者{} ({} @ {})：释放共享锁\n", id, this_thread::get_id(), timestamp(system_clock::now()));
        out_file << move(data);
        cout << id << ' ' << aready_read << endl;
        clear_data.notify_one();
        {
            unique_lock<mutex> locker(new_data_protecter);
            fmt::print(fmt::fg(fmt::color::green), "\t读者{} ({} @ {})：等待下一个可读数据\n", id, this_thread::get_id(), timestamp(system_clock::now()));
//            has_new_data.wait(locker);///等待有新的数据可读
        }
        fmt::print(fmt::fg(fmt::color::green), "\t读者{} ({} @ {})：收到有可读数据的通知\n", id, this_thread::get_id(), timestamp(system_clock::now()));
        readed = true;///标识本线程已经读取过数据
    }
    fmt::print(fmt::bg(fmt::color::brown), "\t读者{} ({} @ {})：结束运行\n", id, this_thread::get_id(), timestamp(system_clock::now()));
}

void clear(){///当所有的线程都读取到数据时，将数据链表头的数据进行清理
    fmt::print(fmt::bg(fmt::color::brown), "清理者({} @ {})：开始运行\n", this_thread::get_id(), timestamp(system_clock::now()));
    while(1){///本线程控制最终退出
        {
            unique_lock<shared_mutex> locker(data_protecter);
            fmt::print(fmt::fg(fmt::color::red), "\t\t清理者({} @ {})：等待所有线程读取头部数据以清理数据池\n", this_thread::get_id(), timestamp(system_clock::now()));
//            clear_data.wait(locker, []{return aready_read.compare_exchange_strong(reader_count, 0) == true;});
//            clear_data.wait(locker, []{return aready_read.compare_exchange_weak(reader_count, 0);});
            clear_data.wait(locker, []{return aready_read == reader_count;});
        aready_read.store(0);
            fmt::print(fmt::fg(fmt::color::red), "\t\t清理者({} @ {})：获取独占写锁以清理数据池\n", this_thread::get_id(), timestamp(system_clock::now()));
            data_list.pop_front();
            fmt::print(fmt::fg(fmt::color::green), "\t\t清理者({} @ {})：数据池大小{}\n", this_thread::get_id(), timestamp(system_clock::now()), data_list.size());
            if(data_list.empty()) break;
        }
        fmt::print(fmt::fg(fmt::color::green), "\t\t清理者({} @ {})：释放独占写锁\n", this_thread::get_id(), timestamp(system_clock::now()));
        fmt::print(fmt::fg(fmt::color::green), "\t\t清理者({} @ {})：通知所有读者读取新数据\n", this_thread::get_id(), timestamp(system_clock::now()));
        has_new_data.notify_all();///通知所有读者可以继续读取;
        if(write_end_flag.load()){
            has_data.notify_all();
            cout << "代替写者发送信号" << endl;
        }
    }
    fmt::print(fmt::bg(fmt::color::brown), "\t清理者 ({} @ {})：结束运行，数据池含有数据{}个\n", this_thread::get_id(), timestamp(system_clock::now()), data_list.size());
    clear_end_flag.store(true);
}

void once_clear(once_flag &just_once){
    call_once(just_once, clear);
}

int main(int argc, char *argv[]){
    string path(argv[1]);
    thread(writer, ref(path)).detach();
    once_flag just_once;
    thread(once_clear, ref(just_once)).detach();
    for(int i = 0; i < reader_count; ++i)
        thread(reader, i).detach();
    while(!clear_end_flag.load())///所有的数据都被清除才会结束进程
        this_thread::sleep_for(20ms);
    return 0;
}
