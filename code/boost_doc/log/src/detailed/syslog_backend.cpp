// syslog是什么：是一种用来在互联网协议（TCP/IP）的网络中传递记录档消息的标准。
// 可用于信息系统管理及信息安全审核，将不同类型系统的日志记录集成到集中的存储库中。

// 类unix平台原生支持syslog，log库实现了RFC中的syslog协议，在库层面提供统一支持。
// log库的syslog支持format与日志分级(提供了库优先级和syslog中的优先级的映射)

#include <boost/log/core/record_view.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>

namespace logging = boost::log;
namespace sinks = logging::sinks;
namespace keywords = logging::keywords;

typedef sinks::synchronous_sink<sinks::syslog_backend> sink_t;

void init_native_syslog(){
    boost::shared_ptr<logging::core> core = logging::core::get();
    /*
    boost::shared_ptr<sinks::syslog_backend> backend =
        boost::make_shared<sinks::syslog_backend>(
            keywords::facility = sinks::syslog::user,
            keywords::use_impl = sinks::syslog::native
        );
    */
    boost::shared_ptr<sinks::syslog_backend> backend(new sinks::syslog_backend(
        keywords::facility = keywords::syslog::user,
        keywords::use_impl = keywords::syslog::native
    ));
    // 设置不同优先级之间的转换规则
    backend->set_severity_mapper(sinks::syslog::direct_severity_mapping<int>("Severity"));
    core->add_sink(boost::make_shared<sink_t>(backend));
}

void init_builtin_syslog(){
    boost::shared_ptr<logging::core> core = logging::core::get();
    boost::shared_ptr<sinks::syslog_backend> backend(new sinks::syslog_backend(
        keywords::facility = keywords::syslog::local0,
        keywords::use_impl = keywords::syslog::udp_socket_based
    ));
    // 设置syslog将被发往的地址和端口
    backend->set_target_address("127.0.0.1", 514);
    // 定义一个自定义的优先级转换的映射对象
    sinks::syslog::custom_severity_mapping<std::string> mapping("M_Severity");
    mapping["debug"] = sinks::syslog::debug;
    mapping["normal"] = sinks::syslog::info;
    mapping["warning"] = sinks::syslog::warning;
    mapping["failure"] = sinks::syslog::critical;
    backend->set_severity_mapper(mapping);
}
