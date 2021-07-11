#include <functional>
#include <any>
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
            std::generate(data.begin(), data.end(), [&generator, &distributor] () -> DataType {
                        return static_cast<DataType>(distributor(generator));
                    });
//            std::copy(data.begin(), data.end(), std::ostream_iterator<DataType>(std::cout, " "));
//            std::cout << "生成数据" << data.size() << std::endl;
        }

        void operator()(int thread_count){
            int batch_size = 10'000, size_threshold = 100;
            std::function<DataType(int, int, int)> action = std::bind(&TMP<Generator, Distribution, DataType, length>::manual_mt<DataType>, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            auto hook = [&action, thread_count, batch_size, size_threshold](){return action(thread_count, batch_size, size_threshold);};
//            auto res = benchmark_mt<DataType, decltype(hook)>(std::move(hook));
            benchmark_entry(std::move(hook));
            benchmark_entry(std::move([this](){std::accumulate(data.begin(), data.end(), 0);}));
            ///执行四种不同策略的并行加法库函数
//            std::vector<std::any> policies{std::execution::seq, std::execution::unseq, std::execution::par, std::execution::par_unseq};
//            for(auto& policy : policies)
//                benchmark_entry<DataType, decltype(policy)>(std::move(policy));
            benchmark_entry<DataType, decltype(std::execution::seq)>(std::move(std::execution::seq));
//            benchmark_entry<DataType, decltype(std::execution::par)>(std::execution::par);
//            benchmark_entry<DataType, decltype(std::execution::unseq)>(std::execution::unseq);
//            benchmark_entry<DataType, decltype(std::execution::par_unseq)>(std::execution::par_unseq);
//            return res;
        }


        template<typename ReturnType> ReturnType manual_mt(int thread_count, int batch_size, int size_threshold){
            int maximum_thread = std::thread::hardware_concurrency();
            if(thread_count >= maximum_thread){
                std::cout << "该处理器支持的并行线程为" << maximum_thread << "，已自动调整参数" << std::endl;
                thread_count = maximum_thread - 1;
            }
            std::cout << fmt::format("运行参数：数据规模={}，最大线程数量={}，每线程处理数据量={}", data.size(), thread_count, batch_size) << std::endl;

//            std::packaged_task<DataType(int, int)> task(&TMP::partial_sum);
//            std::cout << std::boolalpha << task.valid() << std::endl; // false
////            vector<std::thread> thread_pool;
//            while(data.size() > size_threshold){
//                vector<DataType> tmp;
//                for(int start = 0; start ; start += size_threshold){
//                    std::future<DataType> res = task.get_future();
//                    task(start, start+size_threshold);
//                    tmp.push_back(std::move(res.get()));
//                }
//                tmp.swap(data);
//            }
            return static_cast<ReturnType>(data.front());
        }


        template<typename ReturnType = void, class Operation_or_Policy> ReturnType benchmark_entry(Operation_or_Policy && operation_or_policy){
            std::cout << fmt::format("使用策略/方法的签名：{}  ", typeid(operation_or_policy).name());
            my_timer<std::chrono::high_resolution_clock, std::chrono::milliseconds> timer;
            if constexpr(std::is_void<ReturnType>::value){
                operation_or_policy();
                return;
            }else{
                ReturnType res = operation_or_policy(std::forward<Operation_or_Policy&&>(operation_or_policy), data.begin(), data.end());
                return res;
            }
        }
//
//        ///传入可调用的函数对象
//        template<typename ReturnType, class Operation> ReturnType benchmark_mt(Operation&& operation){
//            auto start = std::chrono::high_resolution_clock::now();
//            auto res = operation();
//            auto end = std::chrono::high_resolution_clock::now();
//            std::cout << fmt::format("并行加法使用{}策略，耗时{}ms，结果为{}", typeid(ExecutionPolicy).name(), std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() , res) << std::endl;
//        }
//
//        template<typename ReturnType = void, class ExecutionPolicy> ReturnType benchmark(ExecutionPolicy&& policy){
//            auto start = std::chrono::high_resolution_clock::now();
//            auto res = std::reduce(std::forward<ExecutionPolicy&&>(policy), data.begin(), data.end());
//            auto end = std::chrono::high_resolution_clock::now();
//            std::cout << fmt::format("并行加法使用{}策略，耗时{}ms，结果为{}", typeid(ExecutionPolicy).name(), std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() , res) << std::endl;
//        }

        DataType partial_sum(int start, int end){
            return std::accumulate(data.begin()+start, data.begin()+end, static_cast<DataType>(0));
        }
};


int main(){
    //std::cout << TMP<decltype(std::mt19937), decltype(std::uniform_real_distribution), double, 1000000>()(12) << std::endl;
    //std::cout << TMP<std::mt19937, std::uniform_real_distribution<double>, double, 10'000'000>()(12) << std::endl;
    TMP<std::mt19937, std::uniform_real_distribution<double>, double, 10'000'000>()(12);
    return 0;
}
