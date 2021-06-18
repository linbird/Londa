#include <iostream>
#include <string>

#define B *1 
#define K *1024B
#define M *1024K

using namespace std;

class BaseInfo{
    private:
        const string name;
    public:
        BaseInfo(string &name) : name(name){

        }
};

class Node{
    private:
        BaseInfo base_info;//本节点的基本信息
        pair<void*, unsigned int> share_space;//本节点用于和其他程序IPC的空间，地址、长度
    public:
        Node(string &name) : base_info(name){
            get_ipc_space(1M);
        }

        void get_ipc_space(unsigned int size = 64K){
            shmget();
        }
};


int main(){

}
