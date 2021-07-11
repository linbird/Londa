#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <random>
#include <future>

using std::vector;

template <class Generator, class Distribution, typename DataType = int, int length= 100000> class TMP{
    private:
        std::random_device rd;
        std::vector<DataType> data;
    public:
        TMP(){
            data.reserve(length);
            auto generator = Generator(rd());
            auto distributor = Distribution(-122, 123);
//            std::fill(data.begin(), data.end(), [&generator, &distributor] () -> DataType {
//                        return static_cast<DataType>(distributor(generator));
//                    });
            std::generate(data.begin(), data.end(), [&generator, &distributor] () -> DataType {
                        return static_cast<DataType>(distributor(generator));
                    });
        }

        DataType operator()(int thread_count){
            int maximum_thread = std::thread::hardware_concurrency();
            if(thread_count >= maximum_thread){
                std::cout << "该处理器支持的并行线程为" << maximum_thread << "，已自动调整参数" << std::endl;
                thread_count = maximum_thread - 1;
            }
            int batch_size = 10'000, size_threshold = 100;
            std::packaged_task<DataType(int, int)> task(&TMP::partial_sum);
//            vector<std::thread> thread_pool;
            while(data.size() > size_threshold){
                vector<DataType> tmp;
                for(int start = 0; start ; start += size_threshold){
                    std::future<DataType> res = task.get_future();
                    task(start, start+size_threshold);
                    tmp.push_back(std::move(res.get()));
                }
                tmp.swap(data);
            }
            return std::accumulate(data.begin(), data.end(), static_cast<DataType>(0));

        }

        DataType partial_sum(int start, int end){
            return std::accumulate(data.begin()+start, data.begin()+end, static_cast<DataType>(0));
        }
};

int main(){
    //std::cout << TMP<decltype(std::mt19937), decltype(std::uniform_real_distribution), double, 1000000>()(12) << std::endl;
    std::cout << TMP<std::mt19937, std::uniform_real_distribution<double>, double, 1000000>()(12) << std::endl;
    return 0;
}
