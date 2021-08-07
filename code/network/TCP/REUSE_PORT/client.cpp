#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iterator>
#include <netinet/in.h>
#include <string>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <netinet/in.h>

#include <boost/thread/externally_locked_stream.hpp>
typedef  boost::externally_locked_stream<std::ostream> the_ostream;
boost::recursive_mutex terminal_mutex;
the_ostream mcerr(std::cerr, terminal_mutex);
the_ostream mcout(std::cout, terminal_mutex);
using namespace std;

void worker(sockaddr* addr, string& path){
    char buf[INET_ADDRSTRLEN];

    //    mcout << fmt::format("试图连接到{}:{}\n", ((sockaddr_in*)addr)->sin_addr.s_addr);
    mcout << "试图连接到" << inet_ntoa(((sockaddr_in*)addr)->sin_addr) << endl;

    int client_fd = socket(addr->sa_family, SOCK_STREAM, 0);
    int status = connect(client_fd, addr, sizeof(*addr));
    if(status == -1){
        cerr << "建立连接失败" << strerror(errno) << endl;
    }else{
        sockaddr_storage storage;
        socklen_t size = sizeof(storage);
        getsockname(client_fd, (sockaddr*)&storage, &size);
        sockaddr_in* tmp = (sockaddr_in*)&storage;
        //        mcout << fmt::format("客户端{} 建立TCP连接{}:{} <-> {}:{}\n", \
        this_thread::get_id(), inet_ntoa(tmp->sin_addr), ntohs(tmp->sin_port), \
            inet_ntoa(((sockaddr_in*)addr)->sin_addr), ntohs(((sockaddr_in*)addr)->sin_port));
        mcout << this_thread::get_id() << ":  " << inet_ntoa(tmp->sin_addr) << ':' << ntohs(tmp->sin_port) << "<-->" << inet_ntoa(((sockaddr_in*)addr)->sin_addr) << ':' << ntohs(((sockaddr_in*)addr)->sin_port) << endl;
    };
    this_thread::sleep_for(1s);
    fstream file(path);
    if(file.is_open() == false)
        mcout << this_thread::get_id() << " 打开文件失败" << endl;
    cin.rdbuf(file.rdbuf());
    for(string line; getline(cin, line); line.clear()){
        int count = write(client_fd, line.c_str(), line.size());
        //        mcout << fmt::format("{}读入长为{}的一行，写入网络长为{}\n", this_thread::get_id(), line.size(), count);
    }
    close(client_fd);
    mcout << this_thread::get_id() << " 结束" << endl;
}

int main(int argc, char* argv[]){
    sockaddr_in addr;
    inet_pton(AF_INET, argv[1], &addr.sin_addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    vector<thread> thread_pool;

    string path(argv[3]);
    for(int i = 0; i < 12; ++i){
        thread_pool.emplace_back(worker, (sockaddr*)&addr, ref(path));
    }
    for_each(thread_pool.begin(), thread_pool.end(), [](thread& worker){
            worker.join();
    });
    return 0;
}
