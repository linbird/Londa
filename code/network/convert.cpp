#include <netinet/in.h>
#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main(){
    sockaddr_in addr;
    inet_pton(AF_INET, "192.179.1.1", &addr.sin_addr);
    char str_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, str_addr, sizeof(str_addr));
    cout << str_addr << endl;
    return 0;
}
