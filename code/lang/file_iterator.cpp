#include <fstream>
#include <iostream>
#include <iterator>
#include <ios>
#include <locale>
#include <string>
#include <chrono>
#include <sstream>

using namespace std;
using namespace std::chrono_literals;

void skip_space1(ifstream& input, ofstream& out){
    istream_iterator<string> input_begin(input);
    istream_iterator<string> input_end;
    while(input_begin != input_end){
        cout << *input_begin << endl;
        ++input_begin;
    }
}

void fun1(ifstream& input, ofstream& out){
    for(string data; getline(input, data); cout << data << endl);
}

int main(){
    ifstream readed_file("from.cpp", ios_base::in);
    ofstream writeed_file("to.cpp", ios_base::out);

//    skip_space1(readed_file, writeed_file);
//    cout << endl;
    fun1(readed_file, writeed_file);
    cout << endl;
    return 0;
}
