#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

boost::asio::io_service Service;

class Timer
{
    private:
        boost::asio::deadline_timer timer1;
        boost::asio::deadline_timer timer2;
//        boost::asio::strand m_strand;
        boost::asio::io_context::strand m_strand;
        int count;

    public:
        Timer(boost::asio::io_service& ref_service)
            :timer1(ref_service, boost::posix_time::seconds(3))
             , timer2(ref_service, boost::posix_time::seconds(3))
             , m_strand(ref_service)
             , count(0)
        {
            timer1.async_wait(m_strand.wrap(boost::bind(&Timer::Print1, this, boost::asio::placeholders::error)));
            timer2.async_wait(m_strand.wrap(boost::bind(&Timer::Print2, this, boost::asio::placeholders::error)));
        }
        ~Timer()
        {

        }
        void Print1(const boost::system::system_error& error)
        {
            if (count < 5)
            {
                std::cout << "Pints1's count:" << count<<std::endl;
                std::cout << boost::this_thread::get_id() <<std:: endl;
                ++count;
                timer1.expires_at(timer1.expires_at() + boost::posix_time::seconds(1));
                timer1.async_wait(m_strand.wrap(boost::bind(&Timer::Print1, this, boost::asio::placeholders::error)));
            }
            else
            {
                std::cout << "the Error is:" << error.what() << std::endl;
            }
        }

        void Print2(const boost::system::system_error& error)
        {
            if (count < 5)
            {
                std::cout << "Print2's count :" <<count<< std::endl;
                std::cout << boost::this_thread::get_id() << std::endl;
                ++count;
                timer2.expires_at(timer2.expires_at() + boost::posix_time::seconds(1));
                timer2.async_wait(m_strand.wrap(boost::bind(&Timer::Print2, this, boost::asio::placeholders::error)));
            }
            else
            {
                std::cout << "the Error is:" << error.what() << std::endl;
                boost::asio::io_service::work work(Service);
            }
        }

};

int main(int argc, char** argv)
{

    std::cout << boost::this_thread::get_id() << std::endl;
    Timer m_timer(Service);

    std::cout << "异步给主线程运行到io_Service::run之前，知道异步回调函数调完" << std::endl;
    boost::thread t([&](){ std::cout << "\n进入t线程内部执行" << boost::this_thread::get_id() << std::endl;; Service.run();
            std::cout << "t线程内部：异步给主线程运行到io_Service::run之前，知道异步回调函数调完    (验证)！" << std::endl; });
    t.detach();

    std::cout << "异步给主线程运行到io_Service::run之前，知道异步回调函数调完" << std::endl;
    Service.run();
    std::cout << "异步给主线程运行到io_Service::run之前，知道异步回调函数调完    (验证)！" << std::endl;
    getchar();
    return 0;
}
