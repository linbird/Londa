#include <iostream>

using namespace std;


int main(){
    int a = 1;
    char&& i2c = static_cast<char>(a);
    cout << static_cast<void*>(&a) << " : " << a << endl;
    cout << static_cast<void*>(&i2c) << " : " << i2c << endl;
    cout << endl;

    const int b = 2;
    char ci2c = static_cast<char>(b);
    cout << static_cast<void*>(const_cast<int*>(&b)) << " : " << b << endl;
    cout << static_cast<void*>(&ci2c) << " : " << ci2c << endl;
    cout << endl;

    int c = -1;
    unsigned int* pi2pui = reinterpret_cast<unsigned int*>(&c);
    int &cc = c;
    auto unname = reinterpret_cast<unsigned int&>(cc);
    cout << unname << endl;
    cout << static_cast<void*>(&c) << " : " << c << endl;
    cout << static_cast<void*>(pi2pui) << " : " << *pi2pui << endl;
    cout << static_cast<void*>(&pi2pui) << " : " << pi2pui << endl;
    cout << endl;

    const int* x = &a;
    int* pci2pi = const_cast<int*>(x);
    cout << static_cast<void*>(const_cast<int*>(x)) << " : " << x << endl;
    cout << static_cast<void*>(const_cast<int*>(x)) << " : " << *pci2pi << endl;
    cout << static_cast<void*>(pci2pi) << " : " << *pci2pi << endl;
    return 0;
}
