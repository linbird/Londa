#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>
#include <algorithm>
#include <string>
#include <iterator>
#include <thread>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;

void worker(sockaddr* addr){
    int server_fd = socket(addr->sa_family, SOCK_STREAM, 0);
    if(server_fd == -1){
        cerr << "获取匿名套接字描述符失败" << errno << endl;
    }else{
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
        int status = bind(server_fd, addr, sizeof(*addr));
        if(status == -1){
            cerr << "端口复用失败" << errno << endl;
        }else{
            int status = listen(server_fd, 16);
            if(status == 0){
                sockaddr_storage storage;
                socklen_t size = sizeof(storage);
                if(0 > getsockname(server_fd, (sockaddr*)&storage, &size)){
                    cerr << "获取本地地址失败" << strerror(errno);
                }
                if(storage.ss_family == AF_INET){
                    sockaddr_in* addr = (sockaddr_in*)&storage;
                    cout << fmt::format("服务器{}开启监听，监听地址为{}:{}\n", this_thread::get_id(), inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
                }else{
                    sockaddr_in6* addr = (sockaddr_in6*)&storage;
                    char ip[INET6_ADDRSTRLEN];
                    inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(addr));
                    cout << fmt::format("服务器开启监听，监听地址为{}:{}\n", ip, ntohs(addr->sin6_port));
                }
                while(1){
                    sockaddr client_info;
                    socklen_t len;
                    int client_fd = accept(server_fd, &client_info, &len);
                    char buffer[1024];
                    int read_count, all_count = 0;
                    string read_content("");
                    while(0 != (read_count = read(client_fd, buffer, 1024))){
                        all_count += read_count;
                        copy(buffer, buffer+read_count, back_inserter(read_content));
                        memset(buffer, 0, 1024);
                    }
                    cout << this_thread::get_id() << " 读取内容" << read_content << endl;
                }
                close(server_fd);
            }
        }
    }
    return ;
}

int main(int argc, char* argv[]){
    sockaddr_in addr;
    addr.sin_family = (argv[3][0] == '4')? AF_INET: AF_INET6;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(stoi(string(argv[2])));
    for(int i = 0; i < 4; ++i)
        thread(worker, (sockaddr*)&addr).detach();
    this_thread::sleep_for(100s);
    return 0;
}
