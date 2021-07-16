#include <boost/log/core/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;

using namespace logging::trivial;

void init(){
    logging::add_file_log(
        keywords::file_name = "log_%3N.log",
        keywords::rotation_size = 10*1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0,1,2),
        keywords::format = "[%TimeStamp%]: %Message%"
    );
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

int main(){
    init();
    logging::add_common_attributes();//没有这一句就没有时间戳

/*
    src::severity_logger<severity_level> lg;
    BOOST_LOG_SEV(lg, trace) << "A trace severity message";
    BOOST_LOG_SEV(lg, debug) << "A debug severity message";
    BOOST_LOG_SEV(lg, info) << "An informational severity message";
    BOOST_LOG_SEV(lg, warning) << "A warning severity message";
    BOOST_LOG_SEV(lg, error) << "An error severity message";
    BOOST_LOG_SEV(lg, fatal) << "A fatal severity message";
*/
    BOOST_LOG_TRIVIAL(error) << "此条日志无时间戳";
    BOOST_LOG_TRIVIAL(trace) << "this is a trace message";
    BOOST_LOG_TRIVIAL(debug) << "this is a debug message";
    BOOST_LOG_TRIVIAL(info) << "this is a info message";
    BOOST_LOG_TRIVIAL(warning) << "this is a warning message";
    BOOST_LOG_TRIVIAL(error) << "this is a error message";
    BOOST_LOG_TRIVIAL(fatal) << "this is a fatal message";

    return 0;
}
