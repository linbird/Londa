#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <sched.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include <exception>

#include <system_error>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;

int main(int argc, char* argv[]){
    char* path = "/tmp/locak_commucation.sock";
    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    pid_t client_pid = fork();
    if(client_pid == 0){//这是子进程
        cout << "child:" << getpid() << endl;
        int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        int status = connect(client_fd, (sockaddr*)&addr, sizeof(addr));
        if(status < 0){
            cerr << strerror(errno) << endl;
            exit(errno);
        }
        ifstream file(argv[1]);
        for(string line; getline(file, line); line.clear()){
            line.push_back('\n');
            write(client_fd, line.c_str(), line.size());
        }
        close(client_fd);
        cout << "child process end" << endl;
    }else if(client_pid > 0){
        cout << fmt::format("parent{}, child{}\n", getpid(), client_pid) << endl;
        int sock_un_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        bool what = true;
        setsockopt(sock_un_fd, SOL_SOCKET, SO_REUSEPORT, (const void*)&what, sizeof(what));
        int status = bind(sock_un_fd, (sockaddr*)&addr, sizeof(addr));
        if(status < 0){
            cerr << getpid() << ' ' <<  strerror(errno) << endl;
            throw system_error(error_code(), strerror(errno));
        }
        status = listen(sock_un_fd, 12);
        if(status < 0){
            cerr << getpid() << ' ' <<  strerror(errno) << endl;
            throw system_error(error_code(), strerror(errno));
        }
        sockaddr_un client_info;
        socklen_t client_info_size = sizeof(client_info);
        int client_fd;
        while((client_fd = accept(sock_un_fd, (sockaddr*)&client_info, &client_info_size))){
            char buffer[16];
            memset(buffer, 0, 16);
            string data;
            while(read(client_fd, buffer, 16) != 0){
                data.append(move(string(buffer)));
            }
            cout << data << endl << endl;
        }
        close(sock_un_fd);
    }else{
        cerr << strerror(errno) << endl;
    }
    return 0;
}
