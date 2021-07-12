#include <chrono>
#include <thread>
#include <iostream>

int func(int x, int& value){
    std::cout << x << std::endl;
    std::cout << std::endl << value << std::endl;
    value += 20;
    std::cout << std::endl << value << std::endl;
    return value;
}

int main(){
    int value = 1;
    for(int i = 0; i < 3; ++i)
        std::thread(func, i, std::ref(value)).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
