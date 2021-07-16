#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/attribute.hpp>

namespace logging = boost::log;
namespace src = logging::sources;
namespace keywords = logging::keywords;
namespace trivial = logging::trivial;
namespace attrs = logging::attributes;

// 为啥要用global_logger_storage: https://www.boost.org/doc/libs/1_71_0/libs/log/doc/html/log/detailed/sources.html#log.detailed.sources.global_storage
// 规避了放在namespace时C++标准未规定初始化顺序，线程安全，多平台，多编译模块的众多问题
// 为那些不好决定放在那个位置的日志提供了一个默认的存储位置。

// 创建全局的logger
BOOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(logger_name1, src::severity_logger_mt<>)

// 带参数的创建全局logger的宏
BOOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(logger_name2, src::severity_channel_logger<>,
    (keywords::severity = error)(keywords::channel = "channel"))

// 灵活性最大的全局初始化宏，注意和前一个宏一样，他们的参数只在第一次使用到申明的logger时计算一次
// 来初始化对应的logger
BOOST_LOG_GLOBAL_LOGGER_INIT(logger_name3, src::severity_logger_mt){
    src::severity_logger_mt<> lg;
    lg.add_attributes("StopWatch", attrs::timer());
    return lg;
}

// ODR法则(One Definition Rule/ 单定义规则)：https://blog.csdn.net/digu/article/details/1693616
// 单定义规则要求两个或多个定义必须完全相同。
// 在不同的翻译单元中，当且仅当两个定义中每个符号都相同，并且这些符号在两个翻译单元具有相同的
// 意思时，这两个定义才完全相同。（空格和注释不会打破ODR）
// 为了避免违背ODR而出现未定义的行为，应该将logger的声明和初始化分开
/*
 * BOOST_LOG_GLOBAL_LOGGER(logger_name4, logger_type) 在头文件中提供声明
 * BOOST_LOG_GLOBAL_LOGGER_{INTT | DEFAULT | CTOR_ARGS}(logger_name4, logger_type){} 在源文件中实现logger_name4的初始化的工作。
 * 示例：
// my_logger.h
// ===========

BOOST_LOG_GLOBAL_LOGGER(my_logger, src::severity_logger_mt)


// my_logger.cpp
// =============

#include "my_logger.h"

BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, src::severity_logger_mt)
{
    src::severity_logger_mt< > lg;
    lg.add_attribute("StopWatch", attrs::timer());
    return lg;
}

src::severity_logger_mt< >& lg = logger_name4::get();
*/
