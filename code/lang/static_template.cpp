#include <iostream>
#include <memory>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <memory>
#include <string>

class Test : public std::enable_shared_from_this<Test>{
    public:
        Test() = default;
        Test(Test&) = delete;
        Test operator=(Test&) = delete;
        std::shared_ptr<Test> get_shptr(){
            return shared_from_this();
        }

        void helper(){
            std::cout << "没事儿调用老子干啥，傻逼" << std::endl;
        }
};

void simple_test(){
    /// 正确的示例：两个 shared_ptr 对象将会共享同一对象
    /// 此示例来自https://zh.cppreference.com/w/cpp/memory/enable_shared_from_this
    std::shared_ptr<Test> sp1 = std::make_shared<Test>();///这才是正确写法
    std::shared_ptr<Test> x = sp1->get_shptr();
    std::cout << x.use_count() << std::endl;
    x->helper();
}

template <typename ReturnType> class TS : public std::enable_shared_from_this<TS<ReturnType>>{
    public:
        static std::string ss;
        static ReturnType rs;
        static void sv_fun(){
            rs += rs;
        }

        template<typename ParamType> static ReturnType sRt_fun(){
            std::cout << fmt::format("静态模板函数") << std::endl;
            sv_fun();
            return rs;
        }

        std::shared_ptr<TS> info(){
            std::cout << fmt::format("类型名{}\n\t静态字符串成员{}\n\t静态泛型成员{}\n\n", typeid(*this).name(), ss, rs);
            return this->shared_from_this();//https://stackoverflow.com/questions/23478503/enable-shared-from-this-not-working-on-xcode-5
        }
};

using TS_int = TS<int>;
using TS_float = TS<float>;
using TS_string = TS<std::string>;

template<> std::string TS_int::ss = std::string("int :: 1");
template<> std::string TS_float::ss = std::string("float:: 1");
template<> std::string TS_string::ss = std::string("string 1");

template<> int TS_int::rs = 1;
template<> float TS_float::rs = 10;
template<> std::string TS_string::rs = std::string("100");

int main(){
    std::array<std::shared_ptr<TS_int>, 3> asp_int;
    for(auto& sp_int : asp_int){
        sp_int = std::make_shared<TS_int>();
        std::cout << sp_int->info()->sRt_fun<int>() << std::endl;
    }

    std::array<std::shared_ptr<TS_float>, 3> asp_float;
    for(auto& sp_float : asp_float){
        sp_float = std::make_shared<TS_float>();
        std::cout << sp_float->info()->sRt_fun<float>() << std::endl;
    }

    std::array<std::shared_ptr<TS_string>, 3> asp_string;
    for(auto& sp_string : asp_string){
        sp_string = std::make_shared<TS_string>();
        std::cout << sp_string->info()->sRt_fun<std::string>() << std::endl;
    }

    std::cout << asp_int[0].use_count() << std::endl;

    simple_test();
    return 0;
}
