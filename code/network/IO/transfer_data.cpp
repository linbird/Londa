#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>

using namespace std;

struct Data{
    char x[5];
    int y;
    float z;
};

int main(){
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10800);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    Data data{"hell", 12, 3.5};
    pid_t pid = fork();
    if(pid == 0){
        cout << "child process" << endl;
        int client_fd = socket(AF_INET, SOCK_STREAM, 0);
        cout << client_fd << endl;
        while(-1 == connect(client_fd, (sockaddr*)(&server_addr), sizeof(server_addr)));
        int size = send(client_fd, (void*)(&data), sizeof(Data), 0);
        cout << "Send" << size << '/' << sizeof(Data) << endl;
        close(client_fd);
        return 0;
    }else if(pid > 0){
        cout << "parent process" << endl;
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        int status = bind(server_fd, (sockaddr*)(&server_addr), sizeof(server_addr));
        listen(server_fd, 32);
        sockaddr_in request_addr;
        int len = sizeof(request_addr);
        cout << "Server: wating..." << endl;
        int request_fd = accept(server_fd, (sockaddr*)(&request_addr), (socklen_t*)(&len));
        Data new_data, *dst = &new_data;
        char buffer[64] = {0};
        int n = recv(request_fd, buffer, 63, 0);
        memmove(&new_data, buffer, sizeof(Data));
        cout << "Receive " << n << '/' << 64 << "\n\t:" << endl;
        cout << new_data.x << ' ' <<  new_data.y << ' ' << new_data.z << endl;
        return 0;
    }else{

    }
    return 0;
}
