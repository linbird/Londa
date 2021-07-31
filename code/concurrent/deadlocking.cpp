#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono_literals;

mutex protecter1;
mutex protecter2;

void test(int id){
    auto start = chrono::system_clock::now();
    while(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start) < 70ms) {
        cout << id << " 等待获取protecter1" << endl;
        protecter1.lock();
        cout << id << " 等待获取protecter2" << endl;
        protecter2.lock();
        cout << "\t\t" << id << "获取成功"  << endl;
        protecter2.unlock();
        cout << id << " 释放protecter2" << endl;
        protecter1.unlock();
        cout << id << " 释放protecter1" << endl;
    }
}

void block_lock(){
    protecter1.lock()//;
    try{
    lock(protecter1, protecter2);
    }catch(exception& e){
        cout << e.what() << endl;
    }
}

int main(int argc, char *argv[])
{
//    std::vector<std::thread> threads;
//    for(int i = 0; i < 3; ++i)
//        threads.emplace_back(test, i);
//    for(auto& per_thread : threads)
//        per_thread.join();
//    return 0;
    block_lock();
}
