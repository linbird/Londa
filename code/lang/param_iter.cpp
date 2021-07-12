#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

void func(auto begin, auto end){
    std::sort(begin, end);
    std::copy(begin, end, std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    std::copy(end.base(), begin.base(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}

int main(){
    std::vector<int> test{1,2,3,4,5,6};
    std::copy(test.begin(), test.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    func(test.rbegin(), test.rend());
    std::copy(test.begin(), test.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
    return 0;
}
