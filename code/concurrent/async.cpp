#include <iostream>
#include <future>

using namespace std;

int func(){
    cout << "返回值 == " ;
    return 169168;
}

int main(){
    auto f = std::async(std::launch::async, func);
    cout << "开始执行异步任务：" << f.get() << endl;
    ///开始执行异步任务：返回值 == 169168
}
