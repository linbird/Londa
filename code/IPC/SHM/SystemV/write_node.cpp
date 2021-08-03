#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/ipc.h>
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

using namespace std;
using namespace chrono_literals;

struct SHM{
    mutex* protecter;
    char data[1024];
    SHM(){
        cout << __FUNCTION__ << endl;
//        protecter.lock();
//        protecter.unlock();
        memset(data, 0, 1024);
    }
};

//int writer(string path, void* address){
//    fstream file(path);
//    auto origin_buf = cin.rdbuf();
//    cin.rdbuf(file.rdbuf());
//    for(string line; getline(cin, line); line.clear()){
//        cout << line << endl;
//        SHM *shm = (SHM*)address;
//        unique_lock<mutex> locker(shm->protecter);
////        copy(line.begin(), line.end(), (char*)(address) + sizeof(mutex));
//        this_thread::sleep_for(10ms);
//    }
//    cin.rdbuf(origin_buf);
//    return 0;
//}

int main(int argc, char* argv[]){
    const string path(argv[1]);
    key_t id = ftok(path.c_str(), 0);
    cout << id << endl;
    int erron = shmget(id, sizeof(SHM), 0644|IPC_CREAT);
    if(erron == -1){
        cerr << "创建共享内存失败，检查文件"<< path << "是否存在" << endl;
        return -1;
    }
    SHM* address = (SHM*)shmat(id, nullptr, 0);
    address->protecter = new(address) mutex();
    try{
//addres = new(address) SHM;
    }catch(exception& e){
        cout << e.what() << endl;
        return 0;
    }
//    packaged_task<int(string, void*)> task(writer);
//    future<int> count = task.get_future();
//    thread(move(task), path, addres).join();
//    cout << "写进程写入" << count.get() << "个数据" << endl;
    shmdt(address);
    shmctl(id, IPC_RMID, 0);
    return 0;
}
