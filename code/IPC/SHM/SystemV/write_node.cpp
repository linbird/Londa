<<<<<<< HEAD
#include <asm-generic/errno-base.h>
=======
#include <exception>
#include <fstream>
>>>>>>> ee27ffba3cd73b1435b8a236ae6b4ea7a12493b0
#include <iostream>
#include <string>
#include <sstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>
#include <chrono>
#include <string>
#include <thread>
#include <future>
#include <cstring>

#include <mutex>

<<<<<<< HEAD

using namespace std;
using namespace chrono_literals;

class SHM_manger;

///此对象构建在共享内存上,需要使用replacemment new来接受
//为容器定制分配器

template <typename ContainerType> class SHM_manager : public allocator{
	private:
		constexpr int control_size = sizeof(SHM_manger);
		void* data_zone;///存储了指向实际数据的共享内存地址
		atomic<int> ref_count;
	public:

		SHM_manager(const void* address){///
			int use_count = ref_count.fetch_add(1);
			cout <<  "当前内存已被" << use_conut << "个进程使用"  << endl;
				data_zone = address + control_size;
		}
	private:
		~SHM_manager(){
			if(shmdt(data) == -1){
				cerr << "卸载共享内存失败"
			}
			if(shmctl(shm_id, IPC_RMID, 0) == -1){
				cerr << "释放共享内存失败";
			}
		}
};

class SHM_Gen{
	public:
		constexpr void* data;
		SHM_Gen(string& path, int data_size){
			key_t shmid = ftok(path, 0644|IPC_CREAT);
			int status = shmget(shmid, sizeof(SHM_manager)+data_size, 0);
			if(status == -1) throw std::system_error(EPERM, "创建共享内存失败");
			data = shmat(shmid, nullptr, 0);
		}

		~SHM_Gen() = default;
};


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
	SHM_Gen shm(argv[1], 128*1024);
	SHM_manager manager(shm.data);
	packaged_task<int(chrono::seconds, void*)> task(writer);
	future<int> count = task.get_future();
	thread(task, 2s, address).detach();
	cout << "写进程写入" << count.get() << "个数据" << endl;
	return 0;
=======
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
>>>>>>> ee27ffba3cd73b1435b8a236ae6b4ea7a12493b0
}
