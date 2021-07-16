#include <boost/log/core.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace src = boost::log::sources;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;

using namespace boost::log::trivial;

void global_init(){
    logging::add_file_log(
        keywords::file_name = "dedicated_log_%N.log",
        keywords::rotation_size = 1<<20,
        keywords::format = "[%TimeStamp%]: %Message%"
    );

    logging::core::get()->set_filter(logging::trivial::severity > logging::trivial::trace);
}

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(global_logger, src::logger)
int main(){
//    global_init();
//    logging::add_common_attributes();
    //method 1
    src::logger& lg1 = global_logger::get();
    src::logger lg2;
    BOOST_LOG(lg1) << "这也是一条日志";
    BOOST_LOG_SEV(lg2, error) << "这是ERROR日志条目";
    return 0;
}
