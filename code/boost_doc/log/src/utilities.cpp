#include <boost/log/core.hpp>
#include <iostream>
#include <string>

namespace logging = boost::log;

// string_literal
// string_literal是一个仅需包含头文件的在不需改变string情况下的高效类
// 此类是一个basic_string_literal模板类，提供了除了修改外的STL风格的接口
// 支持关系运算和流运算且更容易保证异常安全
// 为方便使用typedef了string_literal和wstring_literal
#include <boost/log/utility/string_literal.hpp>

////////////////////////////////////////////////////////////////////////////////
// Type dispatchers
// 从属性值对象中提取出对应类型的属性值(有相应的回调仿函数)
#include <boost/log/utility/type_dispatch/type_dispatcher.hpp>

// 主要用于在编译期为已知的类型解值
#include <boost/log/utility/type_dispatch/static_type_dispatcher.hpp>
// 使用方法:
// 1. typedef一个MPL类型(用支持的类型列表实例化boost.mpl)
// 2. 定义并实现对模板参数中类型序列中的每一个类型的访问方法构成的一个仿函数对象
// 3. 以BOOST.MPL类型为参数,以仿函数为构造函数参数对static_type_dispatcher模板实例化
// 4. 对一个不知道具体类型但是仿函数对象一定可以解的对象调用其dispatcher成员函数
// 以下为示例
struct my_base{
    virtual ~my_base() {};
    virtual bool dispatch(logging::type_dispatcher& dispatcher) const = 0;
};

template <typename T>
struct my_value : public my_base{
    T m_value;
    explicit my_value(T const& value) : m_value(value) {}
    bool dispatch(logging::type_dispatcher& dispatcher) const{
        logging::type_dispatcher::callback<T> cb = dispatcher.get_callback<T>();
        if(cb){
            cb(m_value);
            return true;
        }else{
            return false;
        }
    }
};

struct print_visitor{
    typedef void result_type;
    void operator() (int const& value) const{
        std::cout << "Received int value: " << value << std::endl;
    }
    void operator() (double const& value) const{
        std::cout << "Received double value: " << value << std::endl;
    }
    void operator() (std::string const& value) const{
        std::cout << "Received string value: " << value << std::endl;
    }
};

bool print(my_base const& value){
    typedef boost::mpl::vector<int, double, std::string> types;
    print_visitor visiter;
    logging::static_type_dispatcher<types> disp(visiter);
    return value.dispatch(disp);
}

//
#include <boost/log/utility/type_dispatch/dynamic_type_dispatcher.hpp>
// 使用方法：使用register_type()注册类型与相应类型的回调函数
// 以下为示例(static_type_dispatcher的等价写法)
void on_int(int const& value){
    std::cout << "Received int value: " << value << std::endl;
}
void on_double(double const& value){
    std::cout << "Received double value: " << value << std::endl;
}
void on_string(std::string const& value){
    std::cout << "Received string value: " << value << std::endl;
}

void init_disp(logging::dynamic_type_dispatcher& disp){
    disp.register_type<int>(&on_int);
    disp.register_type<double>(&on_double);
    disp.register_type<std::string>(&on_string);
}

bool print1(my_base const& value){
    logging::dynamic_type_dispatcher disp;
    init_disp(disp);
    return value.dispatch(disp);
}
////////////////////////////////////////////////////////////////////////////////
// 进程内交互
// 为不同平台下的系统共享资源的名称提供了命名空间
#include <boost/log/utility/ipc/object_name.hpp>
