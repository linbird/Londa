#include <iostream>
#include <utility>

void test_forward(int& x){
    std::cout << "左值引用版本" << std::endl;
}

void test_forward(int&& x){
    std::cout << "右值引用版本" << std::endl;
}

template <typename ParamType> void test(ParamType&& parameter){
    test_forward(std::forward<ParamType>(parameter));
}

int main(){
    int a = 1;
    int &b = a;
    int &&c = 1;
/// a,b,c三个变量的值类别都是左值(locale value)

    test(1);//#2
    test(a);//#1
    test(b);//#1
    test(c);//#2

    test(std::move(a));//#1
    test(std::move(b));//#1
    test(std::move(c));//#2
    return 0;
}
