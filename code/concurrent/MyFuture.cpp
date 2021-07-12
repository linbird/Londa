#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

template<class T>
class MyFutureInner {
    std::atomic_int count_;
    std::mutex mutex_;
    std::condition_variable condition_;
    T value_;
    bool value_set_ = false;
    public:
    MyFutureInner() : count_{1} {}

    int use_count() const {
        return count_.load(std::memory_order_relaxed);
    }

    // get and increase
    int increase_reference() {
        return count_.fetch_add(1, std::memory_order_relaxed);
    }

    // decrease and get
    int decrease_reference() {
        return count_.fetch_add(-1, std::memory_order_acq_rel) - 1;
    }

    void set(T &&value) {
        std::unique_lock<std::mutex> lock{mutex_};
        value_ = std::move(value); // move assignment
        value_set_ = true;
        condition_.notify_all();
    }

    T move() {
        std::unique_lock<std::mutex> lock{mutex_};
        if (!value_set_) {
            condition_.wait(lock);
        }
        return std::move(value_); // move
    }

    T copy() {
        std::unique_lock<std::mutex> lock{mutex_};
        if (!value_set_) {
            condition_.wait(lock);
        }
        return value_; // copy
    }
};

template<class T>
class MyFuture {
    MyFutureInner<T> *inner_ptr_;
    public:
    MyFuture() : inner_ptr_{new MyFutureInner<T>} {}

    // copy
    MyFuture(const MyFuture &future) : inner_ptr_{future.inner_ptr_} {
        inner_ptr_->increase_reference();
    }

    MyFuture &operator=(const MyFuture &) = delete;

    MyFuture(MyFuture &&future) {
        inner_ptr_ = future.inner_ptr_;
        future.inner_ptr_ = nullptr;
    }

    MyFuture &operator=(MyFuture &&) = delete;

    int use_count() const {
        return inner_ptr_->use_count();
    }

    void set(T &&value) {
        inner_ptr_->set(std::move(value));
    }

    T get(bool copy = false) {
        return copy ? inner_ptr_->copy() : inner_ptr_->move();
    }

    ~MyFuture() {
        if (inner_ptr_ != nullptr && inner_ptr_->decrease_reference() == 0) {
            delete inner_ptr_;
        }
    }
};

void thread2_run(MyFuture<std::string> future) {
    future.set(std::string{"foo"});
}

int main() {
    MyFuture<std::string> future;
    std::thread thread2{thread2_run, future};
    std::string result = future.get();
    std::cout << result << std::endl;
    thread2.join();
    return 0;
}
