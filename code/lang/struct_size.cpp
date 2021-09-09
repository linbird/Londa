#include <iostream>

using namespace std;

class Empty{};

class Char{char a;};

class CIi{
    int i;
    char ch;
};

class CLc{
    char ch;
    int i;
};

int main(){
    cout << sizeof(Empty) << endl;
    cout << sizeof(Char) << endl;
    cout << sizeof(CIi) << endl;
    cout << sizeof(CLc) << endl;
    return 0;
}
