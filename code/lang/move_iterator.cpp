/// 移动迭代器在便利的过程中move了数据，但是还是在容器里留下了空壳子
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <iterator>
#include <numeric>
#include <string>

int main()
{
    std::vector<std::string> v{"this", "_", "is", "_", "an", "_", "example"};

    std::cout << v.size() << std::endl;
    auto print_v = [&](auto const rem) {
        std::cout << rem;
        for (const auto& s : v)
            std::cout << std::quoted(s) << ' ';
        std::cout << '\n';
    };

    print_v("Old contents of the vector: ");

    std::string concat = std::accumulate(std::make_move_iterator(v.begin()),
            std::make_move_iterator(v.end()),
            std::string());

    /* 使用 std::move_iterator 的替代方式可能是：
       using moviter_t = std::move_iterator<std::vector<std::string>::iterator>;
       std::string concat = std::accumulate(moviter_t(v.begin()),
       moviter_t(v.end()),
       std::string()); */
    std::cout << v.size() << std::endl;
    v.shrink_to_fit();
    std::cout << v.size() << std::endl;

    print_v("New contents of the vector: ");

    std::cout << "Concatenated as string: " << quoted(concat) << '\n';
    return 0;
}
