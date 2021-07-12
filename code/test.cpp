#include <iterator>
#include <algorithm>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

int main(){
    random_device rd();
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(generator);
    std::vector<int> v(100);
    generate(v.begin(), v.end(), [&distribution](){return distribution()});
    auto start = v.begin(), end = v.end();
    cout << distance(start + v.size() + 10, end) << endl;
    return 0;
}
