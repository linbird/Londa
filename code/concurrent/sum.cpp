#include <functional>
#include <mutex>
#include <condition_variable>
#include <any>
#include <ctime>
#include <ios>
#include <iostream>
#include <numeric>
#include <execution>
#include <fmt/core.h>
#include <chrono>
#include <iterator>
#include <algorithm>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <random>
#include <future>
#include <mutex>
#include <fmt/ostream.h>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

using namespace std::chrono_literals;

class fixed_thread_pool {
    public:
        explicit fixed_thread_pool(size_t thread_count)
        : data_(std::make_shared<data>()) {///创建任务队列
            for (size_t i = 0; i < thread_count; ++i) {///实例化thread_count个线程
                std::thread([data = data_] {///每个线程都持有任务队列的共享式智能指针
                    std::unique_lock<std::mutex> lk(data->mtx_);///对任务队列的数据进行保护
                    for (;;) {
                        if (!data->tasks_.empty()) {///还有未被执行的任务
                            auto current = std::move(data->tasks_.front());///从任务队列中获取一个等待执行的任务
                            data->tasks_.pop();///移除一个被选择执行的任务
                            lk.unlock();///解锁任务队列
                            current();///执行任务
                            lk.lock();///避免本线程再次获取到任务，但是出了本线程后该锁会自动解锁
                        } else if (data->is_shutdown_) {///如果所有任务执行完了且要求结束线程（is_shutdown_ = true）;
                            break;
                        } else {
                            data->cond_.wait(lk);///等待任务队列中有新的线程或其他来自条件变量的通知
                        }
                    }
                }).detach();
            }
        }

        fixed_thread_pool() = default;
        fixed_thread_pool(fixed_thread_pool&&) = default;

        ~fixed_thread_pool() {
            if ((bool) data_) {///如果还有线程持有共享智能指针（即还有线程没有结束）
                {
                    std::lock_guard<std::mutex> lk(data_->mtx_);///互斥访问data_数据
                    data_->is_shutdown_ = true;///结束任务的设置标志
                }
                data_->cond_.notify_all();///通知所有线程结束自己的任务
            }
        }

        template <class F> void execute(F&& task) {
            {
                std::lock_guard<std::mutex> lk(data_->mtx_);///RAII锁，保护data
                data_->tasks_.emplace(std::forward<F>(task));///项任务队列中加入待执行的任务
            }
            data_->cond_.notify_one();///通知等待队列中的第一个线程
        }

    private:
        struct data {
            std::mutex mtx_;///互斥保护任务队列中的数据
            std::condition_variable cond_;
            bool is_shutdown_ = false;
            std::queue<std::function<void()>> tasks_;///保存待执行任务的任务队列
        };
        std::shared_ptr<data> data_;
};


template<class Clocker, class Unit> class my_timer{
private:
//    using time_point = typename Clocker::time_point;
//    time_point start;
    typename Clocker::time_point start;
public:
    my_timer(){
        start = Clocker::now();
    }

    ~my_timer(){
        typename Clocker::time_point end = Clocker::now();
//        time_point end = Clocker::now();
        typename Clocker::duration draft = end - start;
        std::cout << fmt::format("算法耗时{}", std::chrono::duration_cast<Unit>(draft).count()) << std::endl;
    }
};

template <class Generator, class Distribution, typename DataType = int, int length= 100000> class TMP{
    private:
        std::random_device rd;
        std::vector<DataType> data;
    public:
        typedef DataType type;
        TMP() : data(length){
            auto generator = Generator(rd());
            auto distributor = Distribution(-122, 123);
            std::cout << fmt::format("开始生成数据");
            std::thread([](){
                    std::cout << "*";
                    std::this_thread::sleep_for(1ms);
                    }).detach();
            std::generate(data.begin(), data.end(), [&generator, &distributor] () -> DataType {
                        return static_cast<DataType>(distributor(generator));
                    });

//            std::copy(data.begin(), data.end(), std::ostream_iterator<DataType>(std::cout, " "));
            std::cout << fmt::format("成功生成{}个数据\n", data.size());
        }

        void operator()(int thread_count){
            int batch_size = 30'00, size_threshold = 100;
            std::function<DataType(int, int, int)> action = std::bind(&TMP<Generator, Distribution, DataType, length>::manual_mt<DataType>, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            auto hook = [&action, thread_count, batch_size, size_threshold](){return action(thread_count, batch_size, size_threshold);};
//            auto res = benchmark_mt<DataType, decltype(hook)>(std::move(hook));
//            benchmark_entry(std::move([this](){return std::accumulate(data.begin(), data.end(), static_cast<DataType>(0));}));
//            ///执行四种不同策略的并行加法库函数
////            std::vector<std::any> policies{std::execution::seq, std::execution::unseq, std::execution::par, std::execution::par_unseq};
////            for(auto& policy : policies)
////                benchmark_entry<DataType, decltype(policy)>(std::move(policy));
//            benchmark_entry<DataType, decltype(std::execution::seq)>(std::move(std::execution::seq));
//            benchmark_entry<DataType, decltype(std::execution::par)>(std::move(std::execution::par));
//            benchmark_entry<DataType, decltype(std::execution::unseq)>(std::move(std::execution::unseq));
//            benchmark_entry<DataType, decltype(std::execution::par_unseq)>(std::move(std::execution::par_unseq));
            benchmark_entry(std::move(hook));
        }


        template<typename ReturnType> ReturnType manual_mt(int thread_count, int batch_size, int size_threshold){
            int maximum_thread = std::thread::hardware_concurrency();
            if(thread_count >= maximum_thread){
                std::cout << "该处理器支持的并行线程为" << maximum_thread << "，已自动调整参数" << std::endl;
                thread_count = maximum_thread - 1;
            }
            std::cout << fmt::format("运行参数：数据规模={}，最大线程数量={}，每线程处理数据量={}", data.size(), thread_count, batch_size) << std::endl;

            std::vector<DataType> local_date(data);

            fixed_thread_pool threadpool(thread_count);

            while (local_date.size() >= batch_size) {
                int epoch_count = std::ceil(local_date.size()/static_cast<float>(batch_size));
                int epoch_last = local_date.size()%batch_size;
                int offset = (epoch_last == 0)? batch_size: epoch_last;
                auto start = local_date.begin();
                using Iterator_Category = decltype(start);
/*
                /// async异步串行实现
                std::vector<DataType> next_data;
                do{
                    std::future<DataType> ret = std::async(&TMP<Generator, Distribution, DataType, length>::partial_sum<Iterator_Category>, this, start, start + offset);
                    start = start + offset;
                    offset = batch_size;
                    next_data.push_back(std::move(ret.get()));
                }while(--epoch_count);
                local_date.swap(next_data);
            }
*/

/*
///计算是多线程，回写是单线程有阻塞
                std::vector<DataType> next_data;
                do{
                    std::packaged_task<DataType(Iterator_Category, Iterator_Category)> task(std::bind(&TMP::partial_sum<Iterator_Category>, this, std::placeholders::_1, std::placeholders::_2));
                    std::future<DataType> ret = task.get_future();
                    std::thread(std::move(task), start, start + offset).detach();///缺陷：由于需要在每个线程里都要使用task构建thread，因此task可能需要被反复构建
                    start = start + offset;
                    offset = batch_size;
                    next_data.push_back(std::move(ret.get()));//当线程没有写回结果的时候会阻塞当前调用
                }while(--epoch_count);
                local_date.swap(next_data);
*/

///无法正常完成功能
                std::vector<DataType> next_data;
                do{
                    std::packaged_task<DataType(Iterator_Category, Iterator_Category)> task(std::bind(&TMP::partial_sum<Iterator_Category>, this, std::placeholders::_1, std::placeholders::_2));
                    threadpool.execute([&task, start, offset, &next_data](){
                            std::chrono::system_clock::time_point current_t = std::chrono::system_clock::now();
                            std::time_t currett_time = std::chrono::system_clock::to_time_t(current_t);
std::cout << fmt::format("{}\t线程{}开始工作", std::ctime(&currett_time),std::this_thread::get_id()) << std::endl;
//std::future<DataType> ret = task.get_future();
//                        task(start, start+offset);
//next_data.push_back(std::move(ret.get()));
                        std::this_thread::sleep_for(100ms);
                            current_t = std::chrono::system_clock::now();
                            currett_time = std::chrono::system_clock::to_time_t(current_t);
std::cout << fmt::format("{}: \t线程{}结束工作", std::ctime(&currett_time),std::this_thread::get_id()) << std::endl;
                    });
                    start = start + offset;
                    offset = batch_size;
                }while(--epoch_count);
                local_date.swap(next_data);
            }

std::this_thread::sleep_for(100ms);
            return std::accumulate(local_date.begin(), local_date.end(), static_cast<DataType>(0));
        }


        template<typename ReturnType = void, class Operation_or_Policy> ReturnType benchmark_entry(Operation_or_Policy&& operation_or_policy){
            std::cout << fmt::format("使用策略/方法的签名：{}  ", typeid(operation_or_policy).name())  << std::endl;
            if constexpr(std::is_void<ReturnType>::value){
                DataType res = 0;
                {
                    my_timer<std::chrono::high_resolution_clock, std::chrono::milliseconds> timer;
                    res = operation_or_policy();
                }
                std::cout << fmt::format("计算结果为{}\n\n", res);
                return;
            }else{
                //        std::cout << std::boolalpha << std::is_lvalue_reference<decltype(operation_or_policy)>::value << std::endl;
                //        std::cout << std::boolalpha << std::is_rvalue_reference<decltype(operation_or_policy)>::value << std::endl;
                //        std::cout << std::boolalpha << std::is_reference<decltype(operation_or_policy)>::value << std::endl;
                ReturnType res = 0;
                {
                    my_timer<std::chrono::high_resolution_clock, std::chrono::milliseconds> timer;
                    res = std::reduce(std::forward<Operation_or_Policy&>(operation_or_policy), data.begin(), data.end());
                }
                std::cout << fmt::format("计算结果为{}\n\n", res);
                return res;
            }
        }

        template<typename ForwardIterator> DataType partial_sum(ForwardIterator begin, ForwardIterator end){
            std::cout << fmt::format("线程{}开始计算部分和",std::this_thread::get_id());
            return static_cast<DataType>(0);
//            return std::accumulate(begin, end, static_cast<DataType>(0));
        }
};


int main(){
    //std::cout << TMP<decltype(std::mt19937), decltype(std::uniform_real_distribution), double, 1000000>()(12) << std::endl;
    //std::cout << TMP<std::mt19937, std::uniform_real_distribution<double>, double, 10'000'000>()(12) << std::endl;
    TMP<std::mt19937, std::uniform_real_distribution<double>, double, 1'000'000>()(12);
    return 0;
}
