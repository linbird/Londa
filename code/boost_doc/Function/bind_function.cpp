#include <iostream>
#include <functional>

int Func(int x, int y){
    std::cout << "Func" << x+y << std::endl;
    return x + y;
}

void f(int a, int b, int c){
    std::cout << "f" << a+b+c << std::endl;
}

class A
{
    public:
        int Func(int x, int y){

            std::cout << "A::Func" << x+y << std::endl;
            return x + y;
        }
};

int main(){
    A a;
    auto bf1 = std::bind(Func, 10, std::placeholders::_1);
    auto dd = bf1(20); ///< same as Func(10, 20)
    auto bf2 = std::bind(&A::Func, a, std::placeholders::_1, std::placeholders::_2);
    auto cc = bf2(10, 20); ///< same as a.Func(10, 20)

    std::function< int(int)> bf3 = std::bind(&A::Func, a, std::placeholders::_1, 100);
    auto icc = bf3(10); ///< same as a.Func(10, 100)

    int n1 = 1, n2 = 2, n3 = 3;
    std::function<void()> bound_f = std::bind(f, n1, std::ref(n2), std::cref(n3));
   bound_f();
    return 0;
}
