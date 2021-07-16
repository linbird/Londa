#include <boost/log/core.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions/formatters/stream.hpp>
#include <boost/log/expressions/message.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <exception>
#include <fstream>
#include <ostream>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <boost/log/attributes/attribute_name.hpp>

namespace logging = boost::log;
namespace sinks = logging::sinks;
namespace keywords = logging::keywords;
namespace expr = logging::expressions;
namespace attrs = logging::attributes;
namespace src = logging::sources;

//  Text IPC message queue backend
// 为本机下的多个运行进程之间的log之间的交流提供支持
// 用reliable_message_queue初始化text_ipc_message_backend;
#include <boost/log/sinks/text_ipc_message_queue_backend.hpp>
#include <boost/log/utility/ipc/reliable_message_queue.hpp>

BOOST_LOG_ATTRIBUTE_KEYWORD(a_timestamp, "TimeStamp", attrs::local_clock::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(a_process_id, "ProcessID", attrs::current_process_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(a_thread_id, "ThreadID", attrs::current_thread_id::value_type)

typedef logging::ipc::reliable_message_queue queue_t;
typedef sinks::text_ipc_message_queue_backend<queue_t> backend_t;
typedef sinks::synchronous_sink<backend_t> sink_t;

void process1(){
    try{
        boost::shared_ptr<sink_t> sink = boost::make_shared<sink_t>(
            keywords::name = logging::ipc::object_name(
                logging::ipc::object_name::user, "ipc_message_queue"
            ),
            keywords::open_mode = logging::open_mode::open_or_create,
            keywords::capacity = 256,
            keywords::block_size = 1024,
            keywords::overflow_policy = queue_t::block_on_overflow
        );
        sink->set_formatter(expr::stream << "[" << a_timestamp << "] ["
            << a_process_id <<  ":" << a_thread_id << "]" << expr::smessage
        );
        logging::core::get()->add_sink(sink);
        //  添加TimeStamp, ProcessID and ThreadID等
        logging::add_common_attributes();
        src::logger logger;
        for(unsigned int i = 0; i < 10; ++i){
            BOOST_LOG(logger) << "Message #" << i;
        }
    }catch(std::exception& e){
        std::cout << "Failure: " << e.what() << std::endl;
    }
}

void process2(){
    try{
        queue_t queue(
            keywords::name = logging::ipc::object_name(
                logging::ipc::object_name::user, "ipc_message_queue"
            ),
            keywords::open_mode = logging::open_mode::open_or_create,
            keywords::capacity = 256,
            keywords::block_size = 1024,
            keywords::overflow_policy = queue_t::block_on_overflow
        );
        std::cout << "Viewer process running..." << std::endl;
        std::string message;
        while(queue.receive(message) == queue_t::succeeded){
            std::cout << message << std::endl;
            message.clear();
        }
    }catch(std::exception& e){
        std::cout << "Failure: " << e.what() << std::endl;
    }
}

int main(){
    process1();
}
