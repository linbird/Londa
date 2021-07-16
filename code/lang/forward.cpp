#include <iostream>
#include <type_traits>
#include <utility>

void test_forward(int& x){
    std::cout << "左值引用版本" << std::endl;
}

/*
///此函数的存在会与&、&&版本的函数重载出现二义性
void test_forward(int x){//此函数签名中的x可以接受int, int&&, int&
    std::cout << "左值版本" << std::endl;
}
*/

void test_forward(int&& x){
    std::cout << "右值引用版本" << std::endl;
}

template <typename ParamType> void test(ParamType&& parameter){
    test_forward(std::forward<ParamType&&>(parameter));//错误写法
    test_forward(std::forward<ParamType&>(parameter));//错误写法
    test_forward(std::forward<ParamType>(parameter));
    std::cout << std::endl;
}

int main(){
    int a = 1;
    int &b = a;
    int &&c = 1;

/*
/// a,b,c三个变量的值类别都是左值(locale
//value)，所以接下来三个函数调用都将匹配到左值版本
    test(a);
    test_forward(a);
    test(b);
    test_forward(b);
    test(c);
    test_forward(c);

    test(1);
    test_forward(1);
/// 由a,b,c三个变量创建的三个匿名无名右值引用都是右值，所以接下来三个函数调用都将匹配到右值版本
    test(std::move(a));
    test_forward(std::move(a));
    test(std::move(b));
    test_forward(std::move(b));
    test(std::move(c));
    test_forward(std::move(c));
*/
    test(1);
    test(a);
    test(b);
    test(c);
    test(std::move(a));
    return 0;
}
