#include <boost/progress.hpp>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<int> v(100, 0);
    boost::progress_display pd(20);
    for (auto& x : v)
    {
        v[x] ++;
        sleep(1);
        ++pd;
    }
    return 0;
}
