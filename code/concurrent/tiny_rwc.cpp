#include <ios>
#include <thread>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>

//#include <boost/log/core.hpp>

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using std::thread;
//using namespace boost::log;

vector<string> data_pool;
shared_timed_mutex data_mutex;

const int reader_count = 5;
const int threshold = 10;
const int max_size = 20;

void do_write(string& path){
    ifstream file(path);
    for(string line; getline(file, line); line.clear()){
        {
            unique_lock<shared_timed_mutex> locker(data_mutex, defer_lock);
            locker.lock();
            while(data_pool.size() >= max_size){
                locker.unlock();
                this_thread::sleep_for(10ms);
                locker.lock();
            }
            while(data_pool.size() >= threshold){
                locker.unlock();
                this_thread::sleep_for(5ms);
                locker.lock();
            }
            data_pool.push_back(line);
        }
    }
}

void writer(once_flag& just_once, string& path){
    call_once(just_once, do_write, path);
}

void do_clean(){

}

void clear(once_flag& just_once){
    call_once(just_once, do_clean);
}

void reader(const int id){
    thread_local int index = 0;
    while(1){
        {
            shared_lock<shared_timed_mutex> locker(data_mutex);
            if(index < data_pool.size()){
//                cout << id << ": " << data_pool[index++] << endl;
                printf("%d: %s\n", id, data_pool[index++].c_str());
            }
        }
    }
}

int main(int argc, char *argv[]){
    once_flag once_write, once_clean;
    string path(argv[1]);
    thread(clear, ref(once_clean)).detach();
    thread(writer, ref(once_write), ref(path)).detach();
    for(int i = 0; i < reader_count; ++i)
        thread(reader, i).detach();
    this_thread::sleep_for(1s);
    return 0;
}
