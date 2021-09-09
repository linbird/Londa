#include <iostream>

using namespace std;

class Virtual_Base{//8
	public:
		virtual void func() = 0;
};

class B : public Virtual_Base{};//8
class C : virtual public Virtual_Base{};//8
class D: virtual public Virtual_Base{};//8
class E : public C, public D{};//16

class Void_Base{};
class X : virtual public Void_Base{};
class Y: virtual public Void_Base{};
class Z : public X, public Y{};

int main(){
	cout << sizeof(Virtual_Base) << endl;
	cout << sizeof(B) << endl;
	cout << sizeof(C) << endl; 
	cout << sizeof(D) << endl;
	cout << sizeof(E) << endl;

	cout << sizeof(Void_Base) << endl;
	cout << sizeof(X) << endl;
	cout << sizeof(Y) << endl; 
	cout << sizeof(Z) << endl;
	return 0;
}
