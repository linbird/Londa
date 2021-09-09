#include <iostream>

using namespace std;

class Test{
    int i = 12;
    public:
        int fun(int c, int d);
        int fun(int c, int d) const;
        int just_one() const;
};

int Test::fun(int c, int f){
    cout << i << ' ';
    cout << "mutable" << endl;
return 0;
}

int Test::fun(int c, int f) const{
    cout << i << ' ';
    cout << "const" << endl;
return 0;
}

int Test::just_one() const{
    cout << "just one const" << endl;
    return 0;
}

int main(){
    const Test t1;
    Test t2;
    t1.fun(1, 2);
    t1.just_one();
    t2.fun(1, 2);
    t2.just_one();
    return 0;
}
