// 64bit OS
class Myclass{int var;
    public:
//        virtual void fun();
};						//4

//void Myclass::fun(){};
class MyclassA :virtual public Myclass{int varA;};		//16=4+4+8
class MyclassB :virtual public Myclass{int varB;};		//16=4+4+8
class MyclassC :public MyclassB, public MyclassA{int varC;};	//40=16+16+4+4

int main(){
    Myclass base;
    MyclassA basea;
    MyclassB baseb;
    MyclassC derived;
    return 0;
}
