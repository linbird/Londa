#include <chrono>
#include <iostream>
#include <format>
#include <numeric>
#include <type_traits>
#include <vector>
#include <random>
#include <thread>

using std::vector;

template <typename Type = int> class PallelSum{
    private:
        int thread_count = std::thread::hardware_concurrency()-1;
        vector<std::thread> thread_pool;
        int batch_size = 200;
        vector<Type> data;
        int data_length;
        std::chrono::duration<double> simple_time;
        std::chrono::duration<double> paral_time;
        std::chrono::duration<double> sequenced_time;
        float simple_sum;
        float paral_sum;
        float sequenced_sum;
    public:
        PallelSum(int data_length = 10000){
            this->data_length = data_length;
            generte_data(1, 1000);
            thread_pool.reserve(thread_count);
            std::cout << std::format("初始化长度的为{}的待求和数据", data_length);
        }

        void generte_data(Type minium, Type maximum){
            data.reserve(data_length);
            fill(data.begin(), data.end(), [minium, maximum, this](){
                return do_generate(minium, maximum);
            });
        }

        Type do_generate(Type minium, Type maximum){
            static std::random_device rd;
            static std::mt19937 engine(rd());
            Type one_data;
            if constexpr(std::is_integral<Type>::value){
                std::uniform_int_distribution<Type> dis(minium, maximum);
                one_data = dis(engine);
            }else{
                std::uniform_int_distribution<Type> dis(minium, maximum);
                one_data = dis(engine);
            }
            return one_data;
        }

        template <class ExecutionPolicy> void generte_data(ExecutionPolicy&& policy,Type minium, Type maximum){
            data.reserve(data_length);
            generate(policy, data.begin(), data.end(), [minium, maximum, this](){
                return do_generate(minium, maximum);
            });
        }

        template<typename RType> RType cal(){
            auto start = std::chrono::high_resolution_clock::now();
            while(data.size() > batch_size){
            }
            auto end = std::chrono::high_resolution_clock::now();
            RType sum = accumulate(data.begin(), data.end(), static_cast<RType>(0));
            std::chrono::duration<double> diff = end-start;
            return sum;
        }

        void benchmark(){
            auto start = std::chrono::high_resolution_clock::now();
            simple_sum = std::accumulate(data.begin(), data.end(), 0.0);
            auto middle = std::chrono::high_resolution_clock::now();
            sequenced_sum = std::accumulate();
            auto end = std::chrono::high_resolution_clock::now();
            simple_time = middle - start;
            sequenced_time = end - middle;
        }



        ~PallelSum(){
            std::cout << std::format("{}个数据", data_length) << std::endl;
            std::cout << std::format("单线程：在{}内得到计算结果{}", simple_time, simple_sum) << std::endl;
            std::cout << std::format("手写多线程：{}个线程在{}内得到计算结果{}", thread_count, paral_time, paral_sum) << std::endl;
            std::cout << std::format("并行策略：在{}内得到计算结果{}", sequenced_time, sequenced_sum) << std::endl;
        }
};

int main(){
    PallelSum<>().cal<float>();
    return 0;
}
