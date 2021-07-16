#include <fstream>
#include <iostream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/vector.hpp> //序列化STL容器要导入
#include<boost/serialization/map.hpp> //序列化STL容器要导入

#include <string>
#include <vector>
#include <map>

class Data{
    private:
        int a;
        std::string _string;
        std::vector<int> vi;
        std::map<std::string, int> msi;
//        const static char* str;
        friend class boost::serialization::access;
        template <class Archive>
            void serialize(Archive &ar, const unsigned int version){
//                ar & *str & _string & vi & msi;
                ar & _string & vi & msi;
            }

    public:
        Data()
            : a(1)
              , _string("Hello World")
              , vi({1,2,3,4,5})
              , msi({{"1",1},{"2",2}}){
              }

        void show(){
            std::cout << _string << std::endl;
        }
};

//const char* Data::str = "hello world";

int main(){
    Data a;
    std::ofstream fout("test");
    boost::archive::text_oarchive oa(fout);
    oa << a;
    fout.close();
    std::ifstream fin("test");
    boost::archive::text_iarchive oi(fin);
    Data b;
    oi >> b;
    fin.close();
    b.show();
    return 0;
}
