#include <array>
#include <iterator>
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;

int main(){
    array<int, 11> data_pool;
    for(int i = 0; i < 20; ++i){
        data_pool.fill(i);
        copy(data_pool.begin(), data_pool.end(), ostream_iterator<int>(cout, " "));
        cout << data_pool.size() << endl;
    }
    return 0;
}
