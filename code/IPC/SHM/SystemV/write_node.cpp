#include <iostream>
#include <string>
#include <sstream>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <string>
#include <thread>
#include <future>
#include <cstring>

#include <mutex>

class SHM_manager{
    private;
    ket_t 
};

using namespace std;
using namespace chrono_literals;

int writer(chrono::seconds& limit, void* address){
    auto start_point = chrono::steady_clock::now();
    auto draft = start_point - start_point;
    int index = 0;
    stringstream data_ssr;
    data_ssr << "进程(" << getpid() << ")线程(" << this_thread::get_id() << "写入数据：" ;
    do{
        stringstream data(data_ssr);
        data << index;
        cout << data.str() << endl;
        strncpy(address, data.str().data(), data.str().size());
        ++index;
        draft = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_point);
    }while(draft <= limit);
    return index;
}

int main(int argc, char* argv[]){
    const string path(argv[1]);
    key_t id = ftok(path.c_str(), 0);
    int erron = shmget(id, 128*1024, 0644);
    if(erron == -1){
        cerr << "创建共享内存失败，检查文件"<< path << "是否存在" << endl;
        return -1;
    }
    void* address = shmat(id, nullptr, 0);
    packaged_task<int(chrono::seconds, void*)> task(writer);
    future<int> count = task.get_future();
    thread(task, 2s, address).detach();
    cout << "写进程写入" << count.get() << "个数据" << endl;
    return 0;
}
