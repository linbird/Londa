#include <bits/c++config.h>
#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/attribute_value_impl.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/core/record.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes.hpp>
#include <boost/move/utility_core.hpp>
#include <string>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/channel_feature.hpp>
#include <boost/log/sources/channel_logger.hpp>

namespace logging = boost::log;
namespace src = logging::sources;
namespace keywords = logging::keywords;
namespace attr = logging::attributes;

enum custom_severity{
    normal,
    notification,
    warning,
    error,
    critical
};

// 基本的logger：只提供了一些基本能力，可以用来作为事件通知和警告信息
class network_connection{
private:
    src::logger m_logger;
    logging::attribute_set::iterator m_remore_addr;
public:
    void on_connection(std::string const& remote_addr){
        m_remore_addr = m_logger.add_attribute("RemoteAddress", attr::constant<std::string>(remote_addr)).first;
        if(logging::record rec = m_logger.open_record()){
            rec.attribute_values().insert("Message", attr::make_attribute_value(std::string("Connection established")));
            m_logger.push_record(boost::move(rec));
        }
    }

    void on_disconnect(){
        BOOST_LOG(m_logger) << "Connection shut down";
        m_logger.remove_attribute(m_remore_addr);
    }

    void on_data_received(std::size_t size){
        BOOST_LOG(m_logger) << logging::add_value("ReceivedSize", size) << "Some data received";
    }

    void on_data_send(std::size_t size){
        BOOST_LOG(m_logger) << logging::add_value("SendSize", size) << "Some data sent";
    }
};

void test_simple_logger(){
    network_connection nc;
    nc.on_connection("192.168.1.1:1000");
    nc.on_data_received(100);
    nc.on_data_send(200);
    nc.on_disconnect();
}

// {w}_severity_logger_{mt}提供了日志的分级支持（自动添加了Severity属性，其值默认为int类型）
void severity_logger_impl(){
    // 设置默认的日志等级
    src::severity_logger<custom_severity> slg(keywords::severity = custom_severity::normal);
    BOOST_LOG(slg) << "默认日志等级";
    BOOST_LOG_SEV(slg, custom_severity::error) << "Error级别日志";
    BOOST_LOG_SEV(slg, custom_severity::critical) << "Critical级别日志";
    // 上述宏的函数展开(带/不带默认日志级别):
    logging::record rec = slg.open_record(keywords::severity = custom_severity::normal);
    if(rec){
        logging::record_ostream strm(rec);
        strm << "这是默认等级normal";
        strm.flush();
        slg.push_record(boost::move(rec));
    }
}

// {w}_channel_logger_{mt}可以将日志与应用中的特定模块绑定，其它部分和severity_logger相同.
class network_connection_channel{
private:
    src::channel_logger<> m_net, m_stat;
    logging::attribute_set::iterator m_net_remote_addr, m_stat_remote_addr;
public:
    network_connection_channel()
        : m_net(keywords::channel = "net"),
        m_stat(keywords::channel = "stat"){}

    void on_connection(std::string const& remote_addr){
        attr::constant<std::string> addr(remote_addr);
        m_net_remote_addr = m_net.add_attribute("RemoteAddress", addr).first;
        m_stat_remote_addr = m_stat.add_attribute("RemoteAddress", addr).first;
        BOOST_LOG(m_net) << "默认: Connection established";
    }

    void on_disconnect(){
        BOOST_LOG(m_net) << "Connection shut down";
        m_net.remove_attribute(m_net_remote_addr);
        m_stat.remove_attribute(m_stat_remote_addr);
    }

    void on_data_received(std::size_t size){
        BOOST_LOG(m_stat) << logging::add_value("ReceivedSize", size) << "Some data received";
    }

    void on_data_send(std::size_t size){
        BOOST_LOG(m_stat) << logging::add_value("SendSize", size) << "Some data sent";
    }
};

// logger的异常处理
/*
#include <boost/log/sources/exception_handler_feature.hpp>

enum severity_level{
    normal,
    warning,
    error
};

// A logger class that allows to intercept exceptions and supports severity level
class my_logger_mt :
    public src::basic_composite_logger<char,my_logger_mt,
        src::multi_thread_model< boost::shared_mutex >,
        src::features<src::severity< severity_level >, src::exception_handler>>{
    BOOST_LOG_FORWARD_LOGGER_MEMBERS(my_logger_mt)
};

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, my_logger_mt){
    my_logger_mt lg;
    // Set up exception handler: all exceptions that occur while
    // logging through this logger, will be suppressed
    lg.set_exception_handler(logging::make_exception_suppressor());
    return lg;
}

void logging_function(){
    // This will not throw
    BOOST_LOG_SEV(my_logger::get(), normal) << "Hello, world";
}
*/

// {w}_severity_channel_logger_{mt}结合两种feaure的模板logger
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
// 两个模板参数为日志分级和channel属性名
typedef src::severity_channel_logger_mt<custom_severity, std::string> custom_logger_mt;
BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(custom_logger, custom_logger_mt){
    return custom_logger_mt(keywords::channel = "custom_logger");
}

void via_global_logger(){
    BOOST_LOG_SEV(custom_logger::get(), normal) << "Hello World";
}

int main(int argc, char *argv[]){
    return 0;
}
