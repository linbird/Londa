#include <iostream>
#include <sched.h>
#include <sys/mman.h>
#include <string>
#include <numeric>

#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include <semaphore.h>

using namespace std;

int main(int argc, char* argv[]){
//    ifstream src(argv[1], ios::in);
//    ofstream dst(argv[2], ios::out);
    int share_fd = open(argv[3], O_RDWR);
    if(share_fd < 0){
        cerr << __LINE__ << strerror(errno) << endl;
    }
//    if((src.is_open() && dst.is_open())){
//        cerr << "打开为文件失败" << endl;
//    }
    sem_t* sem = sem_open(argv[4], O_CREAT, O_RDWR, 1);
    if(sem == SEM_FAILED){
        cerr << __LINE__ << strerror(errno) << endl;
    }
    pid_t pid = fork();
    if(pid == 0){//子进程
//        src.close();
        char* share_zone = (char*)mmap(nullptr, 2*4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, share_fd, 0);/////////////
        if(share_zone == MAP_FAILED){
            cerr << __LINE__ << strerror(errno) << endl;
        }
        int last = 1023;
        while(1){
            sem_wait(sem);
            if(share_zone[last] != 0){
                write(1, share_zone, 1024);
                share_zone += 1024;
            }
            sem_post(sem);
        }
        munmap(share_zone, 2*4*1024);
    }else if(pid > 0){
 //       dst.close();
        char* share_zone = (char*)mmap(nullptr, 2*4*1024, PROT_READ | PROT_WRITE, MAP_SHARED, share_fd, 0);/////////////
        if(share_zone == MAP_FAILED){
            cerr << __LINE__ << strerror(errno) << endl;
        }
        char buffer[1024];
        int count = 0;
        while(count < 2*4*1024){
//            iota(buffer, buffer+1024, count);
//            sem_wait(sem);
//            count += 1024;
//            memcpy(share_zone, buffer, 1024*sizeof(char));
//            share_zone += 1024;
//            sem_post(sem);
        }
        munmap(share_zone, 2*4*1024);
    }else{

    }
    return 0;
}
