#include <asm-generic/errno-base.h>
#include <atomic>
#include <bits/c++config.h>
#include <memory>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <system_error>
#include <iostream>
#include <utility>

using namespace std;

//template<typename Container> class SHM_Manager : public allocator<Container>{
//    private:
//        void* control_zone;
//        void* data_zone;
//        size_t control_size;
//        size_t data_size;
//        mutex protecter;
//        Container data_pool;///读写锁保护此内存单元
//    public:
//        pointer allocate(size_type numObjs){
//            return static_cast<pointer>(new(data_zone) );
//        }
//};
//
//class SHM_Gen{
//    private:
//        void* control_zone;
//        void* data_zone;
//        size_t control_size;
//        size_t data_size;
//    public:
//        SHM_Gen(const char* path, size_t size){
//            control_size = sizeof(SHM_Manager);
//            data_size = size;
//            key_t shmid = ftok(path, 0644|IPC_CREAT);
//            int status = shmget(shmid, data_size + control_size, 0644|IPC_CREAT);
//            if(status == -1){
//                string error("获取共享内存失败");
//                cerr << error << endl;
//                throw system_error(EPERM, error.c_str());
//            }
//            control_zone = shmat(shmid, nullptr, 0);
//            data_zone = (void*)((char*)control_zone + control_size);
//        }
//}
//
//#include <new>
//#include <iostream>
//#include <cstdlib> // for exit()
//#include <climits> // for UNIX_MAX
//#include <cstddef>
//
//namespace test {
//
//    template<class T>
//        inline T* _allocate(ptrdiff_t size, T*) {
//            std::cout << "_allocate called" << std::endl;
//            set_new_handler(0);
//            T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));
//            if (NULL == tmp) {
//                std::cerr << "out of memory" << std::endl;
//                exit(0);
//            }
//            return tmp;
//        }
//
//    template<class T>
//        inline void _deallocate(T* p) {
//            ::operator delete(p);
//        }
//
//    template<class T1, class T2>
//        inline void _construct(T1* p, const T2& value) {
//            ::new(p) T1(value);
//        }
//
//    template<class T>
//        inline void _destroy(T* p) {
//            p->~T();
//        }
//
//    template<class T>
//        class Allocator
//        {
//            public:
//                typedef T               value_type;
//                typedef T*              pointer;
//                typedef const T*        const_pointer;
//                typedef T&              reference;
//                typedef const T&        const_reference;
//                typedef size_t          size_type;
//                typedef ptrdiff_t       difference_type;
//
//                template<class U>
//                    struct rebind
//                    {
//                        typedef Allocator<U> other;
//                    };
//
//                pointer allocate(size_type n, const void* hint=0) {
//                    return _allocate((difference_type)n, (pointer)0);
//                }
//
//                void deallocate(pointer p, size_type n) {
//                    return _deallocate(p);
//                }
//
//                void construct(pointer p, const T& value) {
//                    _construct(p, value);
//                }
//
//                void destroy(pointer p) {
//                    _destroy(p);
//                }
//
//                pointer address(reference x) {
//                    return (pointer)&x;
//                }
//
//                const_pointer address(const_reference x) {
//                    return (const_pointer)&x;
//                }
//
//                size_type max_size() const {
//                    return size_type(UINT_MAX/sizeof(T));
//                }
//        };
//
//} // end of namespace

//#include <mutex>
//
//template <typename Container_Type>
//class SHM_Manager{
//    public:
//        using Data_Type = typename Container_Type::value_type;
//        mutex protecter;
//        atomic<int> write_count;///写此空间的人
//        atomic<int> read_count;///读此空间的人
//    public:
//        SHM_Manager(){
//
//        }
//
//        auto write(Data_Type&& data){
//            unique_lock<mutex> locker(protecter);
//            return data_pool.push_back(forward(data));
//        }
//
//};
