#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main(){
	cout << sizeof(string) << endl;
	vector<int> data;
	for(int i = 0; i < 10; ++i){
		if(i%5 == 4)	data.reserve(3);
		cout << data.capacity() << ' ';
		data.push_back(i);
		cout << data.capacity() << ' ';
	}
	cout << endl;
	return 0;
}
