#include <ios>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(){
    ofstream file("./data");
    if(!file.is_open()) cout << "打开文件失败" << endl;
    cout.rdbuf(file.rdbuf());
    for(string line; getline(cin, line); line.clear())
        cout << line << endl;
    file.close();
    return 0;
}
