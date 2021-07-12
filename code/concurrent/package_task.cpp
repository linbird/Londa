#include <ios>
#include <cmath>
#include <iostream>
#include <future>
#include <thread>

using namespace std;

int main(){
    std::packaged_task<int(int,int)> task([](int a, int b) {return a+b; });
    std::future<int> ret = task.get_future(); // get future
    task(1,4);
    std::cout << "task_thread:\t" << ret.get() << '\n';///获取异步结果：22
    task.reset();
    ret = task.get_future();
    std::thread(std::move(task), 12, 10).join(); ///线程式调用，由于packaged_task不支持拷贝类操作，所以得用move传递参数
    std::cout << "task_thread:\t" << ret.get() << '\n';///获取异步结果：22
    return 0;
}
