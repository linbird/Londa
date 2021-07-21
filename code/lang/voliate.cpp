#include <vector>
#include <iostream>

using namespace std;

int main(){
    std::vector<int> data;
    volatile int x = data.size();///错误用法，取该值时不会刷新
    for(int i = 0; i < 10; ++i){
        data.push_back(i);
        cout << x << endl;//输出全是0
    }
    return 0;
}
