#include <fstream>
#include <ios>
#include <iostream>

using namespace std;

int main(){
    fstream file("只读", ios_base::in);
    cout << boolalpha << file.is_open() << endl;
    fstream file1("只写", ios_base::out);
    cout << boolalpha << file1.is_open() << endl;
    fstream file2("读写", ios_base::in|ios_base::out);
    cout << boolalpha << file2.is_open() << endl;
    return 0;
}
