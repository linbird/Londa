#include "rb.h"
#include <vector>

using namespace RB;
using namespace std;

int main(){
	unsigned int data_size = 100; 
	vector<float> data(data_size);
	for(int i = 0; i < data_size; ++i)
		data[i].assign();
	RBTree tree(data);
}
