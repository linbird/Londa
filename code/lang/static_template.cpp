#include <iostream>
#include <memory>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <memory>
#include <string>

//template <typename ReturnType> class TS : public std::enable_shared_from_this<template<typename ReturnType> TS<ReturnType>>{
//template <typename ReturnType> class TS : public std::enable_shared_from_this<TS>{
template <typename ReturnType> class TS : public std::enable_shared_from_this<TS<ReturnType>>{
    public:
        static std::string ss;
        static ReturnType rs;
        static void sv_fun(){
            rs += rs;
        }

        template<typename ParamType> static ReturnType sRt_fun(){
            std::cout << fmt::format("静态模板函数") << std::endl;
            return rs;
        }

        std::shared_ptr<TS> info(){
            std::cout << fmt::format("类型名{}\n\t静态字符串成员{}\n\t静态泛型成员{}\n\n", typeid(*this).name(), ss, rs);
            return shared_from_this();
        }
};

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
//    std::array<TS_int, 3> aTS_int;
//    std::array<TS_float, 3> aTS_float;
//    std::array<TS_string, 3> aTS_string;
//    for(auto TS_data : aTS_int) TS_data.info()->sRt_fun<int>();
//    for(auto TS_data : aTS_float) TS_data.info()->sRt_fun<float>();
//    for(auto TS_data : aTS_string) TS_data.info()->sRt_fun<std::string>();

    Test test;
    std::weak_ptr<Test> x(test.get_shptr());
//    std::shared_ptr<Test> x = test.get_shptr();
//    std::cout << x.use_count() << std::endl;
//    x->helper();
    return 0;
}
