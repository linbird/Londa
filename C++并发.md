
<!-- vim-markdown-toc GFM -->

+ [线程管理](#线程管理)
    * [thread对象](#thread对象)
        - [线程创建](#线程创建)
            + [线程运行方式](#线程运行方式)
    * [参数传递](#参数传递)
        - [**NOTE**](#note)
+ [异常处理](#异常处理)
+ [数据共享](#数据共享)
    * [互斥量mutex](#互斥量mutex)
        - [OOP设计](#oop设计)
        - [条件竞争与死锁](#条件竞争与死锁)
            + [条件竞争](#条件竞争)
            + [死锁](#死锁)
                * [死锁避免](#死锁避免)
    * [lock_guard<mutex>](#lock_guardmutex)
    * [unique_lock](#unique_lock)
+ [数据同步](#数据同步)
    * [等待事件](#等待事件)
        - [等待固定时间](#等待固定时间)
        - [等待条件满足](#等待条件满足)
    * [feature](#feature)
        - [std::async](#stdasync)
        - [std::packaged_task](#stdpackaged_task)
        - [future与promise](#future与promise)
    * [shared_future](#shared_future)
    * [函数式编程FP](#函数式编程fp)
    * [CSP（Communicating Sequential Processer）](#cspcommunicating-sequential-processer)
    * [std::experimental](#stdexperimental)
+ [时间与日期](#时间与日期)
    * [std::chrono](#stdchrono)
        - [clock 时钟](#clock-时钟)
        - [duration 时长](#duration-时长)
        - [time_point 时间点](#time_point-时间点)
    * [std::chrono_literals (c++14)](#stdchrono_literals-c14)
+ [内存模型与原子操作](#内存模型与原子操作)
    * [标准原子类型](#标准原子类型)
        - [注意事项](#注意事项)
    * [其他原子类型](#其他原子类型)
        - [指针原子类型std::atomic<T*>](#指针原子类型stdatomict)
        - [自定义原子类型](#自定义原子类型)
    * [原子操作支持](#原子操作支持)
    * [std::memory_order](#stdmemory_order)
        - [memory_order_relaxed](#memory_order_relaxed)
        - [memory_order_release / memory_order_acquire](#memory_order_release--memory_order_acquire)
        - [memory_order_release / memory_order_consume](#memory_order_release--memory_order_consume)
        - [memory_order_seq_cst](#memory_order_seq_cst)
+ [高级线程管理](#高级线程管理)
    * [线程池](#线程池)

<!-- vim-markdown-toc -->

# 线程管理

## thread对象

+ C++11使用thread类来作为存储和管理线程数据的数据结构。
+ thead和unique_ptr一样都是独占所有权的，都**只支持移动而不支持拷贝**
+ 每一个线程都有一个`std::thread::id`类型来作为唯一标识，可以通过`thread::get_id()和std::this_thread::get_id()`获取。
+ 不能以给旧线程合法赋新值的方式来让对象放弃对旧线程的所有权

### 线程创建

+ 支持从普通函数指针、lambda表达式、带有operator()函数的类的实例对象(**使用临时变量时注意C++的语法解析**)、类的函数(需提供类的实例化的对象的地址)来创建线程。
+ 线程所持有的资源会在线程正常退出的时候自动释放
+ 进程内可以创建的线程的数量是有限的，通过`thread::hardware_concurrency()`函数可以获取支持的线程的具体数量。

### 线程运行

* 线程开始运行的时间是不确定的，这是由操作系统的调度策略决定的。它可能在决定线程运行方式的代码执行前就已经完成了线程的任务。
1. thread::join():子线程会阻塞主线程的执行，只有当子线程结束后主线程才能继续执行。
2. thread::datach():分离式启动线程，线程不会阻塞主线程的运行，子线程在后台运行，可用于创建守护线程。

#### 参数传递

+ 线程的参数传递经过了**两个过程（前者是拷贝传值，后置传递右值）**，一是由thread()构造函数传入thread对象的内部，二是由线程对象内部传递给实际执行线程函数参数。
+ 只需要将需要传递的参数做为thread构造函数的参数即可实现传参，此时参数会被拷贝到线程对象的内存空间，然后在线程内部以右值的方式传递给线程函数

##### **NOTE**

1. thread对象支持用带参数的函数初始化，但是参数中的默认参数会被忽略。
2. 当确实需要以数据的引用方式传递参数时，需要使用std::ref(obj)来生成引用包裹以传递引用类型。
3. 当thread传递不允许拷贝的参数时，可以对原始参数进行move操作以实现参数的正常传递
4. 当参数使用到指针的时候要注意指针所指向的变量的生命周期

### 线程结束

* 当线程对象超出作用域时，由于RAII的特性，线程对象的析构函数会被自动调用。
* 如果thread析构函数被调用时，线程对象还没有join()或者detach()，析构函数会调用`std::terminate()`从而导致进程异常退出。

## 线程管理

### 让渡`yield`

* 在当前线程

# 异常处理

+ 通常需要在线程内部的异常处理代码中手动加入thread::join()代码以确保线程在发生意外后任然可以正常结束

# 数据共享

+ 只有多个线程中存在至少一个对共享数据的修改操作没，就有可能引发数据的错误
+ 需要在多个线程中保持一致的值称为不变量，多个线运行情况下需要保证只能有一个线程(即修改不变量的线程)可以看见不变量的中间状态

## 互斥量mutex

+ mutex实现了锁用来控制对共享数据的访问，提供了lock和unlock两个成员方法实现对数据加锁和解锁，处于两者之间的代码受到了mutex的互斥访问控制。
+ 推荐使用支持RAII的模板类`lock_guard<mutex>`来实现对互斥量的自动管理。
+ C++17的`scoped_lock(mutex_objs)`是C++11中的优化实现（此处没有模板类参数是因为C++17实现了模板类参数的自动推导）。

### OOP设计

+ 将mutex和需要保护的数据封装在同一个类中，要求所有成员函数在调用时加锁，在结束时解锁。
+ **NOTE**：需要特别注意类的调用方法的设计，避免向方法的调用者返回需要被保护的数据的指针或者引用，因为**mutex不能检测并保护通过指针或者引用方式访问**本该受保护的数据。同时也要注意任何一种以指针或者引用向外部传递了受保护数据的操作，这些操作都将使数据失去mutex的保护。总的原则就是**切勿将受保护数据的指针或引用传递到互斥锁作用域之外**。

### 条件竞争与死锁

#### 条件竞争

+ 多个线程竞争获取对同一个资源的访问权限
+ 接口设计的不合理也会导致条件竞争。

#### 死锁

+ 由于一个以上的互斥量由于锁定的顺序不当造成了互相锁定导致整个线程无法向前推进。

##### 死锁避免

1. 要求程序中的所有互斥量都要以相同的顺序申请锁。
2. 一次性申请多个锁，使用`std::lock(mutex1, mutex2, ...)`同时锁定多个锁。
3. 建议：
    1. 避免在一个线程里手动获取多个锁，如果有必要则使用`std::lock()`获取多个锁
    2. 如果不能同时获取多个锁，则要求每个线程以固定的顺序获取多个锁
    3. 避免在持有锁的线程中调用外部代码，因为你可能不知道外部代码的具体操作，他们可能也在尝试获取锁从而违背了建议1。
    4. 实现并使用[层次锁](https://gist.github.com/ZhekaiJin/b2c591e5b78f2c4535a781844dd2e12a)，对应用进行分层,并且识别在给定层上所有互斥量。不允许高层对低层已持有该锁的互斥量再次加锁。[更多参考](http://zhengyi.me/2018/04/11/what-is-lock-hierarchy/)。![层次锁](./img/concurrency/layer-lock.gif)

## lock_guard<mutex>

+ 不可复制，不可移动转移所有权

## unique_lock

+ 一种比lock_guard<>更加灵活的RAII类型的锁，不过对应的使用代价也要大一些。
+ 允许延迟锁定(创建时不锁定)
+ 允许限时深度锁定？？？？？？？？？
+ 允许递归锁定，？？？？？？？？？？
+ 支持与条件变量一起使用。
+ 不可复制，但可以通过move转移所有权

# 数据同步

## 等待事件

### 等待固定时间

1. 等待一个时间段，*_for()类函数：`std::this_thread::sleep_for(std::chrono::milliseconds(100))`让线程等待100ms;
2. 等待某一个时间点，*_until()类函数：

```cpp
using namespace std::chrono_literals;
int f();
auto ft = std::async(f);

if (ft.wait_for(1s) == std::future_status::ready){
    std::cout << ft.get();
}
```

### 等待条件满足

+ 使用条件变量`std::condition_variable、std::condition_variable_any`配合互斥量mutex一起工作。

1. 条件变量的notify_one()成员函数会对通过成员函数wait()注册过的所有线程通知相应的事件。
2. 条件变量的wait()成员函数在自己等待的条件未就绪时会解锁互斥量，并将当前线程置于阻塞或等待状态，当收到条件变量的notify_one()通知时，阻塞的线程会被唤醒并尝试对互斥量加锁并检查条件是否满足，满足就持有锁并返回，否则释放锁进入新的等待周期。
3. 条件变量的notify_all()成员函数可以向所有阻塞的线程检测自己的wait()等待的条件是否满足。

## feature

+ 标准库提供了std::future和std::shared_future两个类用于获取线程运行的返回值，其中前者只能与指定的事件关联，后者可以关联多个事件。
+ future和promise都支持传递线程内部的异常，前者不需要特殊设置可以自动粗出异常，后者需要借助promise::set_exception()函数。

### std::async

+ 通过std:sync()可以启动一个后台任务，其具体的启动策略由sync()的控制参数控制。函数的全体参数由控制参数和运行参数两部分组成。std::async的运行返回值由std::feature接收。

1. 控制参数：std::launch类型，std::launch::async要求函数必须异步执行在其他线程上，std::launch::deferred要求函数在future在调用get()或者wait()才实际执行。默认参数是async|deferred;

### std::packaged_task

+ std::packaged_task<>是一个模板类，模板参数是一个函数签名。
+ 该类通过其他符合签名(类型可以不完全匹配，因为可以隐式类型转换)的函数实例化之后就是一个可调用对象，调用该对象就会执行相关的函数或者可调用对象。
+ std::packaged_task<>对象被调用后其返回值作为异步结果存储在std::future中，可以通过其get_future()成员函数获取。

### future与promise

+ std::promise<T>提供了一种机制，可以手动设置std::future<T>中的值
+ std::promise和std::future共同提供了一种机制:future可以阻塞等待线程的数据就绪，promise可以设置提供数据的线程的值,并将future置为就绪态。
+ 通常将promise对象传入到线程内部，线程内部设置pormise的值，线程外部通过的std::promise的get_future()成员函数获取与之相关的std::future对象的值。

## shared_future

+ future**独享同步结果，所有权只能移动**，只能获取一次数据，即调用get之后不能再次调用get。
+ shared_future支持让多个线程等待同一个事件，因为shared_future的实例是可拷贝的，多个shared_future对象可以引用同一结果。

## 函数式编程FP

* FP不会改变外部状态，不修改共享数据就不存在race condition，因此也就没有必要使用锁，可以实现无锁的算法

## CSP（Communicating Sequential Processer）

* CSP中的线程理论上是分开的，没有共享数据，但communication channel允许消息在不同线程间传递
* 每个线程实际上是一个状态机，收到一条消息时就以某种方式更新状态，并且还可能发送消息给其他线程
* 真正的CSP没有共享数据，所有通信通过消息队列传递，但由于C++线程共享地址空间，因此无法强制实现这个要求。
[CSP实现ATM]()

## std::experimental

# 时间与日期

* C++的时钟库有两个版本，一个是std::chrono库，一个是C样式的日期和时间库(如std::time_t)。

## std::chrono

### clock 时钟

* 标准库中提供了三种类型的时钟：
  1. std::chrono::system_clock：本地系统的当前时间 (可以调整)，静态成员函数to_time_t()支持风格转换
  2. std::chrono::steady_clock：不能被调整的，稳定增加的时间
  3. std::chrono::high_resolution_clock：提供最高精度的计时周期，不同标准库实现的行为有差异
* 时钟类必须提供时间值、时间类型、时钟节拍和时钟稳定性**四种**信息

### duration 时长

* 表示时间间隔的模板类，模板参数为节拍数量(Rep)和节拍精度(std::ratio<Num, Denom=1>：表示Num/Denom秒)。
* duration支持四则运算(基于时间的60进制)
* 标准库定义了常用的duration和ratio，可以直接使用。
* 标准库提供了duration_cast<TYPE>(soutrce)实现duration类型的转换。

```cpp
std::chrono::nanoseconds    duration<long long, std::ratio<1, 1000000000>>
std::chrono::microseconds   duration<long long, std::ratio<1, 1000000>>
std::chrono::milliseconds   duration<long long, std::ratio<1, 1000>>
std::chrono::seconds        duration<long long>
std::chrono::minutes        duration<int, std::ratio<60>>
std::chrono::hours          duration<int, std::ratio<3600>>

typedef ratio<1,       1000000000000000000> atto;
typedef ratio<1,          1000000000000000> femto;
typedef ratio<1,             1000000000000> pico;
typedef ratio<1,                1000000000> nano;
typedef ratio<1,                   1000000> micro;
typedef ratio<1,                      1000> milli;
typedef ratio<1,                       100> centi;
typedef ratio<1,                        10> deci;
typedef ratio<                       10, 1> deca;
typedef ratio<                      100, 1> hecto;
typedef ratio<                     1000, 1> kilo;
typedef ratio<                  1000000, 1> mega;
typedef ratio<               1000000000, 1> giga;
typedef ratio<            1000000000000, 1> tera;
typedef ratio<         1000000000000000, 1> peta;
typedef ratio<      1000000000000000000, 1> exa;
```

### time_point 时间点

* `std::chrono::time_point<clock, duration>`是一个表示距离时钟原点的具体时间点的类模板
* 支持加减duration，支持相互之间相减（值为duration）。
## std::chrono_literals (c++14)

* std::chrono_literals通过字面运算符模板提供了时间的后缀支持。
```cpp
constexpr std::chrono::minutes operator ""min(unsigned long long m){
    return std::chrono::minutes(m);
}

auto x = 45min;
auto y = std::chrono::minutes(45);
x == y;//true
```

# 内存模型与原子操作

## 标准原子类型

* 定义在<atomic>头文件中的模板类，主要用于替代mutex实现同步
* 每个原子类型拥有成员函数is_lock_free()用来判断该原子类型是否无锁(std::atomic_flag保证无锁)。
* atomic<>模板类特化了常用的数据类型，通常类型std::atomic<T>的别名就是atomic_T，只有以下几种例外：signed缩写为s，unsigned缩写为u，long long缩写为llong
```cpp
namespace std {
    using atomic_bool = atomic<bool>;
    using atomic_char = std::atomic<char>;
    using atomic_schar = std::atomic<signed char>;
    using atomic_uchar = std::atomic<unsigned char>;
    using atomic_uint = std::atomic<unsigned>;
    using atomic_ushort = std::atomic<unsigned short>;
    using atomic_ulong = std::atomic<unsigned long>;
    using atomic_llong = std::atomic<long long>;
    using atomic_ullong = std::atomic<unsigned long long>;
}
```
### 注意事项

1. 原子类型不允许由另一个原子类型拷贝赋值，但可以用对应的内置类型或者提供的成员函数赋值。
```cpp
T operator=(T desired) noexcept;
T operator=(T desired) volatile noexcept;
atomic& operator=(const atomic&) = delete;
atomic& operator=(const atomic&) volatile = delete;

std::atomic<T>::store // 替换当前值
std::atomic<T>::load // 返回当前值
std::atomic<T>::exchange // 替换值，并返回被替换前的值

// 与期望值比较，不等则将期望值设为原子值并返回false
// 相等则将原子值设为目标值并返回true
// 在缺少CAS（compare-and-exchange）指令的机器上，weak版本在相等时可能替换失败并返回false
// 因此weak版本通常要求循环，而strong版本返回false就能确保不相等
std::atomic<T>::compare_exchange_weak
std::atomic<T>::compare_exchange_strong

std::atomic<T>::fetch_add // 原子加法，返回相加前的值
std::atomic<T>::fetch_sub
std::atomic<T>::fetch_and
std::atomic<T>::fetch_or
std::atomic<T>::fetch_xor
std::atomic<T>::operator++ // 前自增等价于fetch_add(1)+1
std::atomic<T>::operator++(int) // 后自增等价于fetch_add(1)
std::atomic<T>::operator-- // fetch_sub(1)-1
std::atomic<T>::operator--(int) // fetch_sub(1)
std::atomic<T>::operator+= // fetch_add(x)+x
std::atomic<T>::operator-= // fetch_sub(x)-x
std::atomic<T>::operator&= // fetch_and(x)&x
std::atomic<T>::operator|= // fetch_or(x)|x
std::atomic<T>::operator^= // fetch_xor(x)^x
```
2. std::atomic_flag是一个原子布尔类型，也是唯一一个保证无锁的原子类型，只支持ATOM_FLAG_INIT宏初始化为clear()状态，且只支持在set()和clear()之间转换。

## 其他原子类型

### 指针原子类型std::atomic<T*>

### 自定义原子类型

* 该自定义类型必须[可平凡复制](https://zh.cppreference.com/w/cpp/named_req/TriviallyCopyable) (使用`std::is_trivially_copyable`校验, 通俗的说，就是可以直接按字节拷贝的结构称)。
* 自定义类型的原子类型不允许运算操作，只允许`is_lock_free、load、store、exchange、compare_exchange_weak`和`compare_exchange_strong`，以及赋值操作和向自定义类型转换的操作o

## 原子操作支持

* **原子操作**就是对一个内存上变量（或者叫左值）的读取-变更-存储（load-add-store）作为一个整体一次完成。

### 自由函数
* 除了每个类型各自的成员函数，原子操作库还提供了通用的自由函数(函数名多了一个atomic_前缀，参数变为指针类型)
1. 除`std::atomic_is_lock_free`外，每个自由函数有一个`_explicit`后缀版本，`_explicit`自由函数额外接受一个`std::memory_order`参数
2. 自由函数不仅可用于原子类型，还为std::shared_ptr提供了特化版本(C++20弃用了特化，直接允许std::atomic的模板参数为std::shared_ptr)
```cpp
std::atomic<int> i(42);
int j = std::atomic_load(&i); // 等价于i.load()
std::atomic<int> i(42);
std::atomic_load_explicit(&i, std::memory_order_acquire); // i.load(std::memory_order_acquire)
std::shared_ptr<int> p(new int(42));
std::shared_ptr<int> x = std::atomic_load(&p);
std::shared_ptr<int> q;
std::atomic_store(&q, p);
std::atomic<std::shared_ptr<int>> x; // C++20
```

## std::memory_order

### 问题背景
* 多线程读写同一变量需要使用同步机制，最常见的同步机制就是`std::mutex`和`std::atomic`，其中`atomic`通常提供更好的性能。但在指令的执行过程中还面临以下问题：
    1. 在单线程中，由于编译器优化会执行会造成编译出的机器码出现指令重排，此时机器码的顺序和代码的逻辑顺序不同。
    2. CPU部件为了提高效率，在执行指令时使用了乱序、多发射等技术，这也会导致机器码中的指令执行顺序和机器码顺序不同。
    3. 在多核心的CPU上由于cache的设计，不同核心的cache数据可能不同。
* 为了兼顾指令重拍带来的效率提升和阻止乱序执行可能造成的错误，需要手动限制编译器以及CPU对单线程当中的指令执行顺序进行重排的程度，于是引入了`std::memory_order`。
* ~~为了控制多核CPU上对共享的同一个原子变量操作的顺序，消除指令重拍、cache读写机制的影响，标准库提供了对共享的`atomic`变量的`memory_order`支持。~~
* ~~C++11中的内存模型可以在语言这样一个更高层面去控制多核处理器下多线程共享内存访问的控制，从而忽略编译器和硬件的影响提供跨平台的多线程访问控制，实现跨平台多线程。~~
* `std::memory_order`对操作顺序的控制的具体实现是通过**内存屏障memory barrier**（FENCE）来进行的，memory barrier在不同CPU类型上表现不一定一致。

|         枚举值         |                             规则                             |
| :--------------------: | :----------------------------------------------------------: |
| `memory_order_relaxed` |               不对原子操作的执行顺序做任何保证               |
| `memory_order_release` | 本线程中所有之前的原子写操作完成后才能执行本条原子操作（**我后写**） |
| `memory_order_acquire` | 本线程中所有之后的原子读操作只有在本条原子操作完成后才能执行（**我先读**） |
| `memory_order_acq_rel` |                 同时满足`acquire`和`release`                 |
| `memory_order_consume` | 本线程中所有后续与**本原子变量**相关的原子操作只有在本条原子操作完成后执行 |
| `memory_order_seq_cst` |   默认控制，全部原子操作遵循代码顺序，**不允许重排、乱序**   |

### **NOTE：**
1. `memory_order_consume`仅仅考虑对一个`atomic`数据的读写顺序。而其他几种内存顺序都是在操控/安排多个`atomic`数据之间的读写顺序。
2. C++11所规定的这6种模式，**它本身与多线程无关，是限制的单一线程当中指令执行顺序**。这是由于在单线程的环境下指令重拍不会造成逻辑错误（指令重拍的原则是不冲突才会重排，CPU乱序发射但有序写回，详见计算机体系结构），但在多线程下由于数据的共享乱序可能造成逻辑错误，所以需要**限制单线程下的指令重排以避免多线程环境下出现的问题**。

### 与硬件层面的一致性比较

1. `std::memory_order`是C++在语言级别/软件级别提供的规则

？？？？？？？？？？？？？？？？？？？

# 高级线程管理

## 线程池
* 线程的创建和销毁都是需要时间的，当任务的计算时间在线程的生命期中占比较少且任务较多时，为了避免反复创建和释放线程的代价，引入线程池实现。在线程池中线程结束任务后并不被释放而是等待执行下一个任务，以此来避免线程的重复创建与销毁。
* 线程池一般主要由任务队列和工作线程队列构成。任务队列负责存放主线程需要处理的任务，工作线程队列负责从任务队列中取出和运行任务。
* C++目前为止没有提供线程池的支持，需要自己实现[实现参考](https://wangpengcheng.github.io/2019/05/17/cplusplus_theadpool/) 。
*
