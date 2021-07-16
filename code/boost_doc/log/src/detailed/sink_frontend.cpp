#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/log/sinks/sink.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/unlocked_frontend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/unbounded_ordering_queue.hpp>
#include <boost/log/sinks/bounded_fifo_queue.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <ostream>

namespace logging = boost::log;
namespace src = logging::sources;
namespace sinks = logging::sinks;

/*sink前端提供：消息过滤、异常处理、线程同步、消息格式化(文本类记录)
 * sink的前端总有一个和其匹配的后端，库中提供的前端都默认提供了一个相匹配的后端*/

// filter，sink中的filter由core调用will_consume后触发，并由库提供线程安全保护
// set_filter(args) / reset_filter(args)

// formatter：对于文本类(text-based)日志的后端,前端实现了formatter
// set_formatter() / reset_formatter() : 参数支持lambda表达式

// 异常处理:
// 处理或者忽略或者抛出后端处理日志或者前端的filter发生的意外，库保证线程安全
// set_exception_handler() 不允许返回值，一旦发生异常则filter注定失败。

enum severity_level{
    normal,
    warning,
    error
};

//NOTE: 当前端提供的线程安全保证级别小于后端要求的线程安全及别则不会被编译。

// unlocked_sink: 最基础，访问后端时不提供线程同步保护(但设置fiiler是安全的)

// synchronous_sink:在前者的基础上添加锁实现了线程同步,线程可以通过指向后端的指针独占后端的sink,
// 从而可以修改后端的sink，在前端完成对后端的独占之前，其它都后端的访问都会被锁拦截阻塞。

// asynchronous_sink: 实现为消息队列(record queue),造成的延迟可能不适用于调试等场景
// NOTE: 在多模块的程序中应用日志库时，应当避免动态的卸载某些模块，尤其是使用async_sink时
// 1. 此种的sink有一个专有线程负责同步，可以调用sink的stop()方法使前端停止向后端传输record，此时
// 对应的队列里可能还有record，可以使用sink的flush()方法将这些消息写入后端
// 2. 通过在前端的构造函数里加入start_thread=false参数可以避免创建专有线程,此时有两种处理
//  a. 调用前端的run()方法，此方法将在循环处阻塞写入消息，直到调用stop方法
//  b. 周期性的调用feed_records方法,此方法会将队列列中的所有记录处理后返回。
// 3. 队列的调度策略: 库提供了四种调度的策略：{un}bounded_{fifo | ordering}_queue，
//  unbounded_fifo_queue为默认调度策略，小心unbounded的处理速度小于日志的生成速度，bounded策略
//  对于queue满时也有drop_on_overflow和block_on_overflow两种，后者会阻塞线程直到后端处理
typedef sinks::asynchronous_sink<sinks::text_ostream_backend,
    sinks::bounded_fifo_queue<100, sinks::drop_on_overflow> > sink_t;
// 上述语句为只容纳100条记录采取丢弃的FIFO队列的sink定义了一个别名sink_t

// 重排序record: 尤其适用于多线程
typedef sinks::asynchronous_sink<sinks::text_ostream_backend,
        sinks::unbounded_ordering_queue<
        logging::attribute_value_ordering<unsigned int, std::less<unsigned int>>>> sink1_t;
//  上述语句创建类型：以属性中的某一个值类型为int的属性为排序依据，自小向大排序记录
//  初始化一个这样的前端sink
boost::shared_ptr<sinks::text_ostream_backend> backend = boost::make_shared<sinks::text_ostream_backend>();//申明并初始化一个后端
backend->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter));//绑定流
boost::shared_ptr<sink1_t> sink(new sink1_t(backend,
    keywords::order = logging::make_attr_ordering("LineID", std::less<unsigned int>()),
    keywords::ordering_window = boost::posix_time::seconds(1)));
// 构造方法中的ordering_window参数可选，默认值是和系统相关的小值(足够保证到达后端的有序)。
// 为了保证queue中的所有record被写入，调用sink->flush()来清空写入队列。

