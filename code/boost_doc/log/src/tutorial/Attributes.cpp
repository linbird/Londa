#include <boost/log/attributes.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/core.hpp>
#include <boost/log/detail/trivial_keyword.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/core.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/features.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/expressions.hpp>

using namespace boost::log;

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace keywords= boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;

/////////////////////自定义日志级别////////////////////////////////
enum m_severity_level{
    normal,
    notification,
    warning,
    error,
    critical
};

void init(){
    src::severity_logger<m_severity_level> slg;
    BOOST_LOG_SEV(slg, normal) << "日志级别：normal";
    BOOST_LOG_SEV(slg, notification) << "日志级别：notification";
    BOOST_LOG_SEV(slg, m_severity_level::warning) << "日志级别：warning";
    BOOST_LOG_SEV(slg, m_severity_level::error) << "日志级别：error";
    BOOST_LOG_SEV(slg, critical) << "日志级别：critical";
}

////////////////BOOST_LOG_SEV宏的等价形式///////////////////
void manual_loggong(){
    src::severity_logger<m_severity_level> slg;
    logging::record rec = slg.open_record(keywords::severity = normal);
    if(rec){
        logging::record_ostream strm(rec);
        strm << "Some Message";
        strm.flush();
        slg.push_record(std::move(rec));
    }
}
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

//////////////////////add_common_attributes()/////////////////////
void add_common_attributes(){
    boost::shared_ptr<logging::core> core = logging::core::get();
    core->add_global_attribute("LineID", attrs::counter<unsigned int>(1));
    core->add_global_attribute("TimeStamp", attrs::local_clock());
    core->add_global_attribute("Scope", attrs::named_scope());
}
/////////////////Scope属性的使用////////////////////////////
void name_scope_logging(){
    BOOST_LOG_NAMED_SCOPE("命名空间");
    src::severity_logger<m_severity_level> slg;
    BOOST_LOG_SEV(slg, critical) << "自定义命名空间日志";
}
////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

////////////////////增加自定义的属性///////////////////////////////
void tagged_logging(){
    src::severity_logger<m_severity_level> slg;
    slg.add_attribute("Tag", attrs::constant<std::string>("自定义Tag"));
    BOOST_LOG_SEV(slg, critical) << "自定义Tag日志";
}
///////////////////////////////////////////////////////////////////

/////////////////////////仅在该线程中起作用的属性////////////////////////////
void timed_logging(){
    BOOST_LOG_SCOPED_THREAD_ATTR("TimeLine", attrs::timer());
    src::severity_logger<m_severity_level> slg;
    BOOST_LOG_SEV(slg, normal) << "Starting to time nested functions";
    init();
    BOOST_LOG_SEV(slg, normal) << "Stopping to time nested functions";
}
/////////////////////////////////////////////////////////////////////////////

//////////////////////将属性可以应用filter和formater中//////////////////////
/*{
    BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
    BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
    BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
    BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
    BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)
}*/
//////////////////////将属性可以应用filter和formater中//////////////////////
int main(int argc, char *argv[]){
    //    logging::add_common_attributes();//常用的属性：时间和日志条目号，进程号，线程号（多线程时）
//    add_common_attributes();
//    name_scope_logging();
//    tagged_logging();
//    init();
//    timed_logging();
//    init();
    return 0;
}
