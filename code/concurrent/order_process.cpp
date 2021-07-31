#include <mutex>
#include <thread>
#include <iostream>

std::mutex from;
std::mutex to;

void do_from(){
    thread_local int index = 1;
    while(1){
        std::cout << "from" << index++ << std::endl;
        from.unlock();
        to.lock();
    }
}

void do_to(){
    thread_local int index = 1;
    while(1){
        std::cout << "to" << ++index << std::endl;
        to.unlock();
        from.lock();
    }
}

int main(int argc, char *argv[])
{
    std::thread(do_from).detach();
    std::thread(do_to).join();
    return 0;
}
