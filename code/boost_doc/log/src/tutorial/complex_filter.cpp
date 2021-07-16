#include <boost/phoenix/bind.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sink.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

namespace logging = boost::log;
namespace src = logging::sources;
namespace sinks = logging::sinks;
namespace expr = logging::expressions;

enum severity_level{
    normal,
    notification,
    warning,
    error,
    critical
};

// The operator puts a human-friendly representation of the severity level to the stream
std::ostream& operator<< (std::ostream& strm, severity_level level){
    static const char* strings[] ={
        "normal",
        "notification",
        "warning",
        "error",
        "critical"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);
    return strm;
}

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string);

// 自定义的filter
bool custom_filter(logging::value_ref<severity_level, tag::severity> const& level,
        logging::value_ref<std::string, tag::tag_attr> const& tag){
    return level >= warning || tag == "NOTICE";
}

void init(){
    logging::formatter fmt = expr::stream
        << std::setw(3) << std::setfill('0') << line_id << std::setfill(' ')
        << ": <" << severity << ">\t"
        << expr::if_(expr::has_attr(tag_attr))[expr::stream << "[" << tag_attr << "]"]
        << expr::smessage;

    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("full.log"));
    sink->set_formatter(fmt);
    logging::core::get()->add_sink(sink);

    sink = boost::make_shared<text_sink>();
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("important.log"));
    sink->set_formatter(fmt);
    //method 1
//    sink->set_filter(severity >= warning || (expr::has_attr(tag_attr) && tag_attr == "NOTICE"));
    // method 2
 sink->set_filter(boost::phoenix::bind(&custom_filter, severity.or_none(), tag_attr.or_none()));
    logging::core::get()->add_sink(sink);

    logging::add_common_attributes();
}

void gen_logging(){
    src::severity_logger<severity_level> slg;
    BOOST_LOG_SEV(slg, normal) << "日志等级 normal";
    BOOST_LOG_SEV(slg, warning) << "日志等级 warning";
    BOOST_LOG_SEV(slg, critical) << "日志等级 critical";
    BOOST_LOG_SCOPED_THREAD_TAG("Tag", "NOTICE");
    BOOST_LOG_SEV(slg, normal) << "日志等级 normal";
}

int main(int argc, char *argv[]){
    init();
    gen_logging();
    return 0;
}
