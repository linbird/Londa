#include <boost/log/core.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;

int  main(int argc, char *argv[]){
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::error);
    return 0;
}
