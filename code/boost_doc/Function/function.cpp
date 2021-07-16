#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <functional>

class MathCallBack
{
    int ops1,ops2;
    int result;

    public:
    void Add(int a,int b,std::function<void (int)> func)
    {
        ops1 = abs(a);   /* 实际上这个函数可能非常复杂，非常耗时，这样回调更突显作用*/
        ops2 = abs(b);

        result = ops1+ops2;
        func(result);
    }
};

int main()
{
    MathCallBack math;
    int c1 = 0;
    math.Add(1, 3, [&c1](int result) -> void {
            printf("result = %d\n", result);
            c1 = result;
            });
    printf("c1 = %d\n", c1);
    system("pause");
    return 0;
}

