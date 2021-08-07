#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]){
	fstream file(argv[1]);
	cin.rdbuf(file.rdbuf());
	int count = 0;
	for(string line; getline(cin, line); line.clear()){
		count += line.size();
		cout << line.size() << ' ';
	}
	cout << "end" << endl;
	cout << count << endl;
	file.close();
	
	fstream dup_file(argv[1]);
	if(dup_file.is_open() == false)
		cout << "打开文件失败" << endl;
	cin.rdbuf(file.rdbuf());
	while(!dup_file.eof()){
		string line;
		getline(dup_file, line);
		count -= line.size();
		cout << line.size() << ' ';
	}
	cout << "end" << endl;
	cout << count << endl;
	dup_file.close();
	
	fstream dup_file1(argv[1]);
	if(dup_file1.is_open() == false)
		cout << "打开文件失败" << endl;
	cin.rdbuf(dup_file1.rdbuf());
	while(!cin.eof()){
		string line;
		getline(cin, line);
		count -= line.size();
		cout << line.size() << ' ';
//		cin >> ws;
	}
	cout << "end" << endl;
	return 0;
}
