#include <iostream>
#include <cstring>

//template <typename T>
//const T &max(const T &a, const T &b)
//{
//    std::cout << "template max(const T& a, const T& b)" << std::endl;
//    return a < b ? b : a;
//}
//
//// 函数模板重载
//template <typename T>
//const T &max(const T &a, const T &b, const T &c)
//{
//    std::cout << "template max(const T& a, const T& b, const T& c)" << std::endl;
//    return ::max(a, b) < c ? c : ::max(a, b);
//    // ::max 会调用非模板函数
//}
//
//// 非模板函数重载
//const int &max(const int &a, const int &b)
//{
//    std::cout << "max(const int& a, const int& b)" << std::endl;
//    return a < b ? b : a;
//}
//
//// 函数模板特化
//template <>
//const char *const &max(const char *const &a, const char *const &b)
//{
//    std::cout << "template <> max(const char* const&a, const char* const& b)" << std::endl;
//    return strcmp(a, b) < 0 ? b : a;
//}
//
//// 非模板函数重载
//const char *const &max(const char *const &a, const char *const &b)
//{
//    std::cout << "max(const char* const&a, const char* const& b)" << std::endl;
//    return strcmp(a, b) < 0 ? b : a;
//}
//
//class Test
//{
//public:
//
//    friend bool operator<(const Test &t1, const Test &t2)
//    {
//        std::cout << "operator<(const Test& t1, const Test& t2)" << std::endl;
//        return true;
//    }
//};
//
//int main(void)
//{
//    ::max(5.5, 6.6);     // 自动推导 max(const double&, const double&);
//    ::max('a', 'c');     // 自动推导 max(const char&, const char&);
//
//    Test t1, t2;
//    ::max(t1, t2); // Test::operator<(const Test& t1, const Test& t2)
//
//    const char *str1 = "aaa", *str2 = "zzz";
//    ::max(str1, str2); //优先选择非模板函数
//
//    ::max<>(str1, str2); //指定使用模板，进而找到模板特化
//    //  std::cout<<::max<const char*>(str1, str2); // 显式指定模板特化函数max(const char* const&a, const char* const& b)
//    ::max(1, 5, 3); // 模板匹配，进而自动推导
//
//    ::max('a', 50); // 'a'即97;选择非模板函数（char可以隐式转换成int）
//
//    ::max(97, 100);          // 优先选择非模板函数
//
//    ::max<>(97, 100);        // 指定使用模板，进而自动推导
//    //  std::cout<<::max<>('a', 50)<<std::endl;       // Error,指定使用模板，但编译器不知道怎样推导
//    ::max<int>(97, 100); // 显式指定模板函数max(const int&, const int&)
//    ::max<int>('a', 50); // 显式指定模板函数max(const int&, const int&)
//
//
//    return 0;
//}


//template<typename T> void func(T t){
//	std::cout << "template<typename T> void func(" << typeid(T).name() << ' ' << t << ')' << std::endl;
//}
//
//template<> void func(int& t){
//	std::cout << "template<> void func(" << "int& " << t << ')' << std::endl;
//}
//
////template<> void func(int* t){
////	std::cout << "template<> void func(" << "int* " << t << ')' << std::endl;
////}
//
//template<> void func(const int* t){
//	std::cout << "template<> void func(" << "const int* " << t << ')' << std::endl;
//}
//
//int main(int argc, char *argv[])
//{
//    int i(2);
//    func(i);   //version1
//    func(2);
//    func(&i);   //version1
//    return 0;
//}

//#include <iostream>
//using namespace std;
//
//long double operator"" _mm(long double x) { return x / 1000; }
//size_t operator"" _len(char const*, size_t n){    return n;}
//char const* operator"" _r(char const* s){    return s;}
//string operator"" _rs(char const* s){    return "string";}
//unsigned long operator"" _rs(unsigned long long s){
//	cout << "ULL" << s << endl;
//return s;}
//
//int main(){
//    cout << 512_rs << '\n'; // x5y
//	return 0;
//}
//

//#include <map>
//
//using namespace std;
//
//int main(){
//    double fc = 0.0;
//    map<double, int> record;
//    record[1.0/0] = 2;
//    record[-1.0/0] = 3;
//    cout << 1.0/ fc << endl;
//    for(auto [kay, value]: record)
//        cout << kay << ' ' << value << endl;
//
//    cout << -1%10 << endl;
//    return 0;
//}
//
//


#include <random>
#include <iostream>
#include <ctime>


//该函数接受三个参数分别指定随机数范围和种子，返回double
double random_unint(unsigned int min, unsigned int max, unsigned int seed = 0)
{
    static std::default_random_engine e(seed);
    static std::uniform_real_distribution<double> u(min, max);
    return u(e);
}

int main(void)
{
    std::random_device rd;
    for (int i = 0; i < 15; ++i) {
        std::cout << random_unint(0, 15, rd()) << " ";
    }
    std::cout << std::endl;
    return 0;
}
