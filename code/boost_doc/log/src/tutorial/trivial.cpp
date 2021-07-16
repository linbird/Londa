#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/core.hpp>

using namespace boost::log;
/*
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;
 */

void init(){
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
}

int main(){
    init();
    BOOST_LOG_TRIVIAL(trace) << "this is a trace message";
    BOOST_LOG_TRIVIAL(debug) << "this is a debug message";
    BOOST_LOG_TRIVIAL(info) << "this is a info message";
    BOOST_LOG_TRIVIAL(warning) << "this is a warning message";
    BOOST_LOG_TRIVIAL(error) << "this is a error message";
    BOOST_LOG_TRIVIAL(fatal) << "this is a fatal message";
    return 0;
}
