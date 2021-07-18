#include <cwchar>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>

using std::cout;
using std::endl;
using namespace std::chrono;
using namespace std::chrono_literals;

int main(){
    auto current = system_clock::now();
    time_t current_time = system_clock::to_time_t(current);
    cout << std::string(ctime(&current_time)) << endl;///y-m-d-h-m-s
    struct tm* global_time = gmtime(&current_time);
    cout << asctime(global_time) << endl;

    timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", std::gmtime(&ts.tv_sec));
    printf("%s.%09ld UTC\n", buff, ts.tv_nsec);
    //Current time: 12/27/19 09:03:29.497456000 UTC

    return 0;
}
