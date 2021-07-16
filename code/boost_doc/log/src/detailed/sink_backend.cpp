#include <boost/log/core.hpp>
#include <boost/log/core/core.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <fstream>
#include <ostream>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;
namespace sinks = logging::sinks;
namespace keywords = logging::keywords;
namespace expr = logging::expressions;
namespace attr = logging::attributes;

// 文本流后端：可以绑定到多个流，但需要不同的流要求的formatter提供各自的sink
#include <boost/log/sinks/text_ostream_backend.hpp>
void t_s_b(){
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;
    boost::shared_ptr<logging::core> core = logging::core::get();
    boost::shared_ptr<sinks::text_ostream_backend> backend =
        boost::make_shared<sinks::text_ostream_backend>();
    //将后端与多个输出流绑定
    backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    backend->add_stream(boost::shared_ptr<std::ostream>(new std::ofstream("sample.log")));
    backend->auto_flush(true);
    // 将后端绑定到前端sink上
    boost::shared_ptr<sink_t> front_sink(new sink_t(backend));
    core->add_sink(front_sink);
}

// 文本文件后端：相比于前者提供的文件写入，本类后端提供了加强特性：
// 基于文件大小/日期的记录切片、灵活的文件命名、日志管理、auto_flush
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/keywords/file_name.hpp>
#include <boost/log/keywords/target_file_name.hpp>
void t_f_b(){
    typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
    boost::shared_ptr<logging::core> core = logging::core::get();
//切分时间点
// rotation_at_time_point: 通过改变第一个参数可以在每月或每周的某天的某点切分
// rotation_at_time_interval: 按时间间隔切分，如按小时:posix_time::hours(1)
// 自定义一个函数bool fun(void)，再每写入一个log到文件都会询问该函数
// 在后端析构是日志也会自动进行一次切片,可以在构造函数的最后加入上面注释的参数避免

// 文件的命名模式里支持通配符：支持的占位符有：%N(文件计数)，%%%(百分号)，Boost.DateTime中定义的一些表示当前的日期和时间的符号，如%Y-%m-%d_%H-%M-%S等
    boost::shared_ptr<sinks::text_file_backend> backend =
        boost::make_shared<sinks::text_file_backend>(
            keywords::file_name = "file.log", //临时存储日志的文件名
            keywords::target_file_name = "file_%5N.log", //最终存储日志的文件名的命名模式
            keywords::rotation_size = 1<<20, //并不是最终文件大小的精确数值,单位:Byte
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0)// 每天的12:00::00时切分日志文件(同样也不是精确时间，因为切分的动作发生在试图向文件写入新的日志记录时。)
            //,keywords::open_mode = std::ios_base::out | std::ios_base::app // 追加写入
            //,keywords::enable_final_rotation = false // 禁止后端析构时的自动分片文件
            );
// 两个文件的创建时机不同时间戳也就不同，前者创建于打开文件时后者于关闭时。

    boost::shared_ptr<sink_t> front_sink(new sink_t(backend));

// 文件打开/关闭时的一些回调函数(handler)
// 函数签名 void fun(sinks::text_file_backend::stream_type& file)
// 注册方法sink->locked_backend()->set_{open | close}_handler(&fun)
    auto open_hander = [](sinks::text_file_backend::stream_type& file) -> void {};
    auto close_hander = [](sinks::text_file_backend::stream_type& file) -> void {};
    front_sink->set_formatter(
        expr::format("\t<record id=\"%1%\" timestamp=\"%2%\">%3%</record>")
        % expr::attr< unsigned int >("RecordID")
        % expr::attr< boost::posix_time::ptime >("TimeStamp")
        % expr::xml_decor[ expr::stream << expr::smessage ]
    );
    front_sink->locked_backend()->set_open_handler(open_hander);
    front_sink->locked_backend()->set_close_handler(close_hander);

// 日志管理：当多个sink后端使用相同的日志文件名称时，其行为是未定义的。
    front_sink->locked_backend()->set_file_collector(sinks::file::make_collector(
        keywords::target = "logs", // 写入日志的目标文件夹
        keywords::max_size = 1<<20, // 日志文件的总大小上限Byte
        keywords::min_free_space = 1<<21, // 磁盘上的剩余可用空间的下限Byte
        keywords::max_files = 1<<9 // 允许的日志文件数量的上限
    ));

// 旧日志管理：合并旧日志，追加写入模式
    front_sink->locked_backend()->scan_for_files();

    core->add_sink(front_sink);
}

// 多文本文件的后端
#include <boost/log/sinks/text_multifile_backend.hpp>
void t_mf_b(){
    typedef sinks::synchronous_sink<sinks::text_multifile_backend> sink_t;
    boost::shared_ptr<logging::core> core = logging::core::get();
    boost::shared_ptr<sinks::text_multifile_backend> backend =
        boost::make_shared<sinks::text_multifile_backend>();

    // 根据记录中的RequestID将所有记录存储到不同的文件中，此处设置文件名
    // 文件名也可以由自定义的函数对象产生，签名为
    // text_multifile_backend::path_type fun(logging::record const& rec)
    // 此时后端并不清楚一个文件的使用情况，因此需要自己实现文件切片和清理
    backend->set_file_name_composer(
        sinks::file::as_file_name_composer(
            expr::stream << "logs/" << expr::attr< std::string >("RequestID") << ".log")
    );
    boost::shared_ptr<sink_t> front_sink(new sink_t(backend));
    front_sink->set_formatter(expr::stream
        << "[RequestID: " << expr::attr< std::string >("RequestID") << "] " << expr::smessage
    );
    core->add_sink(front_sink);
}
