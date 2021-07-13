#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

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
