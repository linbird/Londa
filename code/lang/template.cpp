#include <iostream>
#include <fmt/core.h>
#include <type_traits>

template<typename Type> class Test{
    public:
        using ParamType = Type;
    Test* self;

    Test* print(){
        self = this;
        std::cout << fmt::format("我就是我、不一样的{}", reinterpret_cast<int>(this)) << std::endl;
//        ret<Test>().print();
        return self;
    }

    template<typename ReturnType> ReturnType ret(){
        return ReturnType();
    }
};

int main(){

    std::cout << fmt::format("我还是我，不变的自我{}", reinterpret_cast<int>(Test<int>().print()->print())) << std::endl;
    std::cout << fmt::format("模板参数类型关系（相等）：{}", typeid(float) == typeid(typename Test<float>::ParamType)) << std::endl;
    return 0;
}
