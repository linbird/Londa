#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/log/attributes/value_extraction.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/core/record_view.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/message.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/formatting_ostream_fwd.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <fstream>
#include <iostream>
#include <boost/log/attributes/value_extraction.hpp>
//#include <boost/optional.hpp>

namespace logging = boost::log;
namespace src = logging::sources;
namespace keywords = logging::keywords;
namespace sinks = logging::sinks;
namespace expr = logging::expressions;


// 设置formatter
//Lambda-style formatters
// expr::attr< logging::trivial::severity_level >("Severity")等价与logging::trivial::severity
void init_method1(){
    logging::add_file_log(
        keywords::file_name = "formatter_%N.log",
        keywords::format = (expr::stream
            << expr::attr<unsigned int>("LineID")
            << ": " << logging::trivial::severity
            << "> " << expr::smessage
        )
    );
}

void init_method2(){
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("formater_%2N.log"));
//Lambda-style formatters
    sink->set_formatter(expr::stream
        << std::hex << std::setw(8) << std::setfill('0') << expr::attr<unsigned int>("LineID")
        << ": " << logging::trivial::severity << "> " << expr::message
    );
/*
//Boost.Format-style formatters
    // This makes the sink to write log records that look like this:
    // 1: <normal> A normal severity message
    // 2: <error> An error severity message
    sink->set_formatter(expr::format("%1%: <%2%> %3%")
        % expr::attr< unsigned int >("LineID")
        % logging::trivial::severity
        % expr::smessage);
*/
    logging::core::get()->add_sink(sink);
}

// YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
void init_style2(){
    logging::add_file_log(
        keywords::file_name = "style2_%N.log",
        keywords::format = (expr::stream
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
            << ": " << logging::trivial::severity << "> " << expr::smessage
        )
    );
}

//被%包围的占位符号代表的属性必须有相应的属性值替换，注意此方法不被后端sink的set_formatter()方法
//接受，必须经过parse_formatter()方法解析后才可以被接受。
void text_style_formatter(){
    logging::add_file_log(
        keywords::file_name = "log/tex_style_%N.log",
        keywords::format = "[%TimeStamp%:] %Message%"
    );
}

// 自定义formatter，其本身是一个函数对象，签名如下:
// void (logging::record_view const& rec, logging::basic_formatting_ostream< CharT >& strm);
void custom_formatter(logging::record_view const& rec, logging::formatting_ostream& strm){
    // 这里取属性有两种写法
    strm << logging::extract<unsigned int>("LineID", rec) << ": "
        << "<" << rec[logging::trivial::severity] << ">" << rec[expr::message];
}

void init_custom_formatter(){
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("formater_%2N.log"));
    sink->set_formatter(&custom_formatter);
    logging::core::get()->add_sink(sink);
}

int main(int argc, char *argv[]){
//    init_method2();
//    init_method1();
    init_custom_formatter();
    logging::add_common_attributes();
    src::severity_logger<logging::trivial::severity_level> lg;
    BOOST_LOG_SEV(lg, boost::log::trivial::trace) << "日志等级：trace";
    BOOST_LOG_SEV(lg, boost::log::trivial::debug) << "日志等级：debug";
    BOOST_LOG_SEV(lg, boost::log::trivial::info) << "日志等级：info";
    BOOST_LOG_SEV(lg, boost::log::trivial::warning) << "日志等级：warning";
    BOOST_LOG_SEV(lg, boost::log::trivial::error) << "日志等级：error";
    BOOST_LOG_SEV(lg, boost::log::trivial::fatal) << "日志等级：fatal";
    return 0;
}
