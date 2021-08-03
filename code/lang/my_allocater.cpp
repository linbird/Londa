#include <new>      // placement new
#include <cstddef>  // ptrdiff_t size_t
#include <cstdlib>  // exit
#include <climits>  // UINT_MAX
#include <iostream> // cerr
#include <ostream>

namespace dcl
{
    // 分配内存
    template<class T>
        inline T* _allocate(ptrdiff_t size, T*) {
            std::set_new_handler(0);
            T* tmp = (T*)(::operator new((size_t)(size * sizeof(T))));

            if(tmp == 0) {
                std::cout<< "out of memory " << std::endl;
                exit(1);
            }
            return tmp;
        }

    // 释放内存
    template<class T>
        inline void _deallocate(T* buffer) {
            ::operator delete(buffer);
        }

    // 以申请的内存中重新构造 T1对象
    template<class T1,class T2>
        inline void _construct(T1* p, const T2& value) {
            std::cout << "\t " << p << " " << value << std::endl;
            new(p) T1(value);
        }

    // placement new 对象的析构
    template<class T>
        inline void _destroy(T* ptr) {
            ptr->~T();
        }

    template <class T>
        class allocator {
            public:
                typedef     T           value_type;
                typedef     T*          pointer;
                typedef     const T*    const_pointer;
                typedef     T&          reference;
                typedef     const T&    const_reference;
                typedef     size_t      size_type;
                typedef     ptrdiff_t   difference_type;

                pointer hole = (pointer)(nullptr);
                pointer start = hole;

                template <class U>
                    struct rebind {
                        typedef allocator<U>    other;
                    };

                // 申请 n 个 pointer类型的空间
                pointer allocate(size_type n,const void* hint = 0) {
                    if(start != hole){
                    }else{
                        auto adddress = _allocate((difference_type)n, (pointer)0);
                        start = adddress;
                    }
                    std::cout << __FUNCTION__ << " n=" << n << " 空间起始地址:  " << start << std::endl;
                    return start;
                }

                // new 空间的析构
                void deallocate(pointer ptr, size_type n) {
                    std::cout << __FUNCTION__ << std::endl;
                    _deallocate(ptr);
                }

                // 已经new 的空间重新分配
                void construct(pointer ptr, const T& value) {
                    std::cout << __FUNCTION__ << " value=" << value << std::endl;
                    _construct(ptr, value);
                }

                // 销毁重新分配的空间
                void destory(pointer p) {
                    _destroy(p);
                }

                // 得到指针变量
                pointer address(reference x) {
                    return (pointer)&x;
                }

                // const 指针变量
                const_pointer const_address(reference x) {
                    return (const_pointer)&x;
                }

                // 得到最大可以开辟的空间数
                size_type max_size() const {
                    return size_type(UINT_MAX / sizeof(T));
                }
        };
}

#include <vector>
#include <iterator>

using namespace std;

int main(){
    int arr[5] {1, 2, 3, 4, 5};
//    std::vector<int, dcl::allocator<int>> v(arr, arr+5);
    vector<int, dcl::allocator<int>> v;
    v.reserve(12);
    for(auto x : arr) v.push_back(x);
//    cout << v.size() << ' ' << v.data() << endl;
    copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
    return 0;
}
