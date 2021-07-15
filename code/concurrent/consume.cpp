#include <future>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>
#include <functional>
#include <iostream>
#include <algorithm>

std::deque<std::packaged_task<int()>> task_q;
std::mutex mu;
std::condition_variable cond;

void thread_1()
{
    std::packaged_task<int()> t;
    {
        // std::lock_guard<std::mutex> locker(mu);
        std::unique_lock<std::mutex> locker(mu);
        cond.wait(locker, [](){ return !task_q.empty();});
        t = std::move(task_q.front());///取出task
        task_q.pop_front();
    }
    t();///执行task
}

int factorial(int x){
    return (x == 1)? x : x*factorial(x-1);
}

int main()
{
    std::thread t1(thread_1);

    std::packaged_task<int()> t(std::bind(factorial, 6));
    std::future<int> res = t.get_future();
    {
        std::lock_guard<std::mutex> locker(mu);
        task_q.push_back(std::move(t));///将任务放入任务队列
    }
    cond.notify_one();///通知等待队列的线程

    t1.join();
    std::cout << "result int main thread: " << res.get() << std::endl;
    return 0;
}
