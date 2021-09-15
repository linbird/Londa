#include <string>
#include <iostream>

using namespace std;

int main(){
    string str;
    for(int i = 0; i < 1024; ++i){
        cout << sizeof(str) << ':' << str.size() << '/' << str.capacity() << ": " << static_cast<void*>(const_cast<char*>((str.data()))) << endl;
        str.push_back(i);
    }
    return 0;
}
