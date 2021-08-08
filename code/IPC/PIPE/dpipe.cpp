#include <cerrno>
#include <cstring>
#include <ios>
#include <iostream>
#include <fstream>
#include <sched.h>
#include <string>

#include <unistd.h>

#include <fmt/format.h>
#include <fmt/ostream.h>

using namespace std;

int main(int argc, char* argv[]){
    int pipe_fd[2];
    if(-1 == pipe(pipe_fd))
        cerr << strerror(errno) << endl;
    int write_fd = pipe_fd[1], read_fd = pipe_fd[0];
    pid_t pid = fork();
    if(pid < 0){
        cerr << "创建子进程失败" << endl;
    }else if(pid == 0){
        close(read_fd);
        ifstream input(argv[1]);
        if(input.is_open() == false){
            cerr << "父进程打开文件失败\n";
        }
        cout << fmt::format("父进程{}打开文件{}\n", getpid(), argv[1]);
        for(string line; getline(input, line); line.clear()){
            line.push_back('\n');
            const char* data = line.c_str();
//            cout << line;
            write(write_fd, data, line.size());
        }
        close(write_fd);
        input.close();
    }else{
        close(write_fd);
        ofstream output("/tmp/output");
        char buffer[1024];
        int read_count = 0;
        while(0 != (read_count = read(read_fd, buffer, 1024))){
            cout << buffer << endl;
            output << buffer;
            memset(buffer, 0, 1024);
        }
        close(read_fd);
        output.close();
    }
    return 0;
}
