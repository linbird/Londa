#include <cstdio>
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

std::time_t getTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    std::time_t timestamp = tmp.count();
    return timestamp;
}
std::tm* gettm(uint64_t timestamp)
{
    uint64_t milli = timestamp;
    milli += (uint64_t)8*60*60*1000;//转换时区，北京时间+8小时
    auto mTime = std::chrono::milliseconds(milli);
    auto tp=std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds>(mTime);
    auto tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* now = std::gmtime(&tt);
    return now;
}

std::string getTimeStr()
{
    time_t timep;
    timep = getTimeStamp();
    struct tm *info;
    info = gettm(timep);

    char tmp[27] = {0};
    std::sprintf(tmp, "[%04d-%02d-%02d %02d:%02d:%02d.%06ld]", info->tm_year+1900, info->tm_mon+1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec, timep%1000000);
    return tmp;
}

int main(){
    std::cout << getTimeStr() << std::endl;
    return 0;
}
