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


// log库中的所有的信息都会在record对象中出现。有多种方法从record对象中提取信息
// 方法一：visit
void via_visitation(logging::record const& rec){
    logging::visit<trivial::severity_level>("Severity", rec, [](trivial::severity_level level){
        std::cout << level << std::endl;
    });
}

// 方法二：extract
void via_extract(logging::record const& rec){
    logging::value_ref<trivial::severity_level> level = logging::extract<trivial::severity_level>("Severity", rec);
    std::cout << level << std::endl;
}

// 方法三：查询属性集
void via_lookup(logging::record const& rec){
    logging::attribute_value_set const& values = rec.attribute_values();
    logging::attribute_value_set::const_iterator it = values.find("Severity");
    if(it != values.end()){
        logging::attribute_value const & value = it->second;
        std::cout << value.extract<trivial::severity_level>() << std::endl;
    }
}

// 方法四：使用subscript
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", trivial::severity_level)
void via_subscript(logging::record const& rec){
    logging::value_ref<trivial::severity_level, tag::severity> const& level = rec[severity];
    std::cout << level << std::endl;
}

// 测试访问record对象
// 在log中，record不可拷贝可move，主要用来记录信息。record_view不可变可拷贝()浅拷贝),用来处理信息.
void via_record(logging::record const& rec){
    src::severity_logger<trivial::severity_level> slg;
    BOOST_LOG_SEV(slg, trivial::warning) << "日志级别: warning";
    via_visitation(rec);
    via_extract(rec);
    via_lookup(rec);
    via_subscript(rec);
}

int main(){
    logging::attribute_set attrs;
    logging::record rec = logging::core::get()->open_record(attrs);
    via_record(rec);
    return 0;
}
