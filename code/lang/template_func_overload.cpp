#include <iostream>
#include <ostream>
#include <utility>

template <typename ParamType> void fun(ParamType&& a){
    std::cout << "右值模板函数" << std::endl;
}

template <typename ParamType> void fun(ParamType& a){
    std::cout << "左值模板函数" << std::endl;
}

template <typename ParamType> void test(ParamType&& parameter){
    std::cout << "模板函数" << std::endl;
    fun(std::forward<ParamType>(parameter));
}

template <> void test(int&& a){
    std::cout << "全特化int&&版本" << std::endl;
}

template <> void test(int& a){
    std::cout << "全特化int&版本" << std::endl;
}

//template <> void test(float a){//error不匹配任何模板
//    std::cout << "全特化float版本" << std::endl;
//}

int main(){
    int a = 1;
    int &b = a;
    int &&c = 1;

    test(a);//#1
    test(b);//#1
    test(c);//#2

    test(1);//#2
    test(std::move(a));//#1
    test(std::move(b));//#1
    test(std::move(c));//#2

    std::cout << std::endl;
    float x = 1.2f;
    test(1.2f);
    test(x);
    test(std::move(x));

    std::cout << std::endl;
    char y = 'c';
    test('c');
    test(y);
    test(std::move(y));
    return 0;
}
