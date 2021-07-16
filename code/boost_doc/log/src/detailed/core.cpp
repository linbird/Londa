#include <boost/log/exceptions.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_value_set.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/core.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/unlocked_frontend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/core/record_view.hpp>
#include <boost/log/attributes/value_visitation.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/attribute_value_set.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace logging = boost::log;
namespace src = logging::sources;
namespace trivial = logging::trivial;
namespace attrs = logging::attributes;
namespace sinks = logging::sinks;
namespace expr = logging::expressions;

// std::excception -> runtime_error/logic_error
struct custom_exception_handler{
    typedef void result_type;
    void operator()(std::runtime_error const& e) const{
        std::cout << "runtime_error: " << e.what() << std::endl;
    }
    void operator()(std::logic_error const& e) const{
        std::cout << "logic_error: " << e.what() << std::endl;
        throw;
    }
};

// log core以单例存在于库，它维护全局和各线程的属性集、为所有的source将record注入核心提供接口\
// 提供全局的意外处理钩子、进行全局过滤、将record根据规则分发到每个sink、提供刷新机制。
void modify_core(){
    boost::shared_ptr<logging::core> core = logging::core::get();
// 修改attributes_set（类似std::map [属性名：指向属性的指针?]）
    // 主要接口：[add|remove]_[global|thread]_attribute;
    // 小心add系列接口返回的迭代器在这些属性被注册到core后会失效。
    std::pair<logging::attribute_set::iterator, bool>
        res = core->add_global_attribute("LineID", attrs::counter<unsigned int>());
    core->remove_global_attribute(res.first);

// 自定义的filter(返回bool的函数对象)
    // global_filter是一个过滤器对象，默认的filter不过滤任何内容。
    // 写法一
    core->set_filter(expr::attr<trivial::severity_level>("Severity") >= trivial::error);
    // 写法二
    /*
    bool custom_filter(logging::value_ref<severity_level, tag::severity> const& level,
        logging::value_ref<std::string, tag::tag_attr> const& tag){
        return level >= warning || tag == "NOTICE";
    }
    core->set_filter(boost::phoenix::bind(&custom_filter, severity.or_none(), tag_attr.or_none()));
    */

// 启用/禁用core的log功能
    core->set_logging_enabled(false);
    core->set_logging_enabled(true);

// 管理sink，在core中sink的执行顺序并不明确
   boost::shared_ptr<sinks::text_ostream_backend>
       backend = boost::make_shared<sinks::text_ostream_backend>();
   backend->add_stream(boost::make_shared<std::ostream>(&std::clog, boost::null_deleter()));
   typedef sinks::unlocked_sink<sinks::text_ostream_backend> sink_t;
   boost::shared_ptr<sink_t> sink = boost::make_shared<sink_t>(backend);
   core->add_sink(sink);
   core->remove_sink(sink);

// 异常处理（无参的函数对象）
// core中提供的异常处理是全局性质的，主要做一些通用的异常处理操作
// sinks和source都提供了异常处理的能力。
    core->set_exception_handler(logging::make_exception_handler<std::runtime_error, std::logic_error>(custom_exception_handler()));

// 为record提供进入核心的接口
    logging::attribute_set attrs;//未初始化
    logging::record rec = core->open_record(attrs);
    if(rec){
        logging::record_ostream strm(rec);
        strm << "这是一条日志";
        strm.flush();
        core->push_record(boost::move(rec));
    }
}

int main(){
    modify_core();
    return 0;
}
