#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <sched.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h>

#include <cerrno>
#include <cstring>

using namespace std;

void on_quit(int signal){
    cout << getpid() << ": 接收到SIGQUIT信号" << signal << endl;
}

int main(int argc, char* argv[]){
    char* path = "/tmp/fifo.pipe";
    int status = mkfifo(path, 0777);
    if(status < 0){
        cerr << strerror(errno) << endl;
    }
    pid_t pid = fork();
    if(pid < 0){
        cerr << strerror(errno) << endl;
    }else if(pid == 0){
        signal(SIGQUIT, on_quit);
        cout << "子进程: " << getpid() << endl;
        int pipe_rfd = open(path, O_RDWR);
        if(pipe_rfd < 0){
            cerr << "子进程" << strerror(errno) << endl;
        }
        ofstream file(argv[2], ios::binary | ios::out);
        if(file.is_open() == false){
            cerr << "子进程" << "error " << argv[1] << endl;
        }
        char buffer[4*1024];
        int count = 0;
        memset(buffer, 0, 4*1024);
        while(0 != (count = read(pipe_rfd, buffer, 4*1024))){
            file.write(buffer, count);
            memset(buffer, 0, 4*1024);
        }
        close(pipe_rfd);
        file.close();
    }else{
        cout << "父进程: " << getpid() << endl;
        int pipe_wfd = open(path, O_RDWR);
        if(pipe_wfd < 0){
            cerr << "父进程" << strerror(errno) << endl;
        }
        ifstream file(argv[1], ios::binary | ios::in);
        if(file.is_open() == false){
            cerr << "父进程：" << "error " << argv[1] << endl;
        }
        char buffer[4*1024];
        while(!file.eof()){
            memset(buffer, 0, 4*1024);
            int count = file.readsome(buffer, 4*1024);
            cout <<  "父进程：" << "读入" << count << "个数据\n";
//            write(2, buffer, count);
//            cout << boolalpha << file.good() << ' ' << file.eof() << ' ' << file.fail() << ' ' << file.bad() << endl;
            if(count == 0) break;
            write(pipe_wfd, buffer, count);
        }
        close(pipe_wfd);
        file.close();
        kill(pid, SIGQUIT);
    }
    return 0;
}
