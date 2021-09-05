#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

int main(){
    unordered_multimap<int, string> omp;
    vector<string> values{"first", "second", "third"};
    for(int i = 0; i < 12; ++i)
        for(auto str : values)
            omp[i] = str;
    cout << omp.size() << endl;
    cout << omp.bucket_count() << endl;
    cout << omp.load_factor() << endl;
    cout << omp.max_load_factor() << endl;
    for(int i = 0; i < 12; ++i){
        auto data = omp.bucket(i);
        cout << endl << "bucket: " << i << endl;
        copy(data.begin(), data.end(), ostream_iterator<string>(cout, ","));
    }

}
