/* server */
#include<sys/epoll.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<pthread.h>
#include <sys/time.h>

#define MAX_EVENTS 1024
#define LISTEN_PORT 33333
#define MAX_BUF 65536

struct echo_data;
int setnonblocking(int sockfd);
int events_handle(int epfd, struct epoll_event ev);
void run();

// 应用TCP保活机制的相关代码
int set_keepalive(int sockfd, int keepalive_time, int keepalive_intvl, int keepalive_probes) {
    int optval;
    socklen_t optlen = sizeof(optval);
    optval = 1;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen)) {
        perror("setsockopt failure.");
        return -1;
    }

    optval = keepalive_probes;
    if (-1 == setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, &optval, optlen)) {
        perror("setsockopt failure.");
        return -1;
    }

    optval = keepalive_intvl;
    if (-1 == setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, &optval, optlen)) {
        perror("setsockopt failure.");
        return -1;
    }

    optval = keepalive_time;
    if (-1 == setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, &optval, optlen)) {
        perror("setsockopt failure.");
        return -1;
    }
}

int main(int _argc, char* _argv[]) {
    run();

    return 0;
}

void run() {
    int epfd = epoll_create1(0);
    if (-1 == epfd) {
        perror("epoll_create1 failure.");
        exit(EXIT_FAILURE);
    }

    char str[INET_ADDRSTRLEN];
    struct sockaddr_in seraddr, cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    seraddr.sin_port = htons(LISTEN_PORT);

    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (-1 == bind(listen_sock, (struct sockaddr*)&seraddr, sizeof(seraddr))) {
        perror("bind server addr failure.");
        exit(EXIT_FAILURE);
    }
    listen(listen_sock, 5);

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &ev)) {
        perror("epoll_ctl add listen_sock failure.");
        exit(EXIT_FAILURE);
    }

    int nfds = 0;
    while (1) {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (-1 == nfds) {
            perror("epoll_wait failure.");
            exit(EXIT_FAILURE);
        }

        for ( int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                int conn_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &cliaddr_len);
                if (-1 == conn_sock) {
                    perror("accept failure.");
                    exit(EXIT_FAILURE);
                }
                printf("accept from %s:%d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
                set_keepalive(conn_sock, 120, 20, 3);
                setnonblocking(conn_sock);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, conn_sock, &ev)) {
                    perror("epoll_ctl add conn_sock failure.");
                    exit(EXIT_FAILURE);
                }
            } else {
                events_handle(epfd, events[n]);
            }
        }
    }

    close(listen_sock);
    close(epfd);
}

int setnonblocking(int sockfd){
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

struct echo_data {
    char* data;
    int fd;
};

int events_handle(int epfd, struct epoll_event ev) {
    printf("events_handle, ev.events = %d\n", ev.events);
    int fd = ev.data.fd;
    if (ev.events & EPOLLIN) {
        char* buf = (char*)malloc(MAX_BUF);
        bzero(buf, MAX_BUF);
        int count = 0;
        int n = 0;
        while (1) {
            n = read(fd, (buf + count), 1024);
            printf("step in edge_trigger, read bytes:%d\n", n);
            if (n > 0) {
                count += n;
            } else if (0 == n) {
                break;
            } else if (n < 0 && EAGAIN == errno) {
                perror("errno == EAGAIN, break.");
                break;
            } else {
                perror("read failure.");
                if (ETIMEDOUT == errno) {
                    if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev)) {
                    perror("epoll_ctl del fd failure.");
                    exit(EXIT_FAILURE);
                    }
                    close(fd);
                    return 0;
                }
                exit(EXIT_FAILURE);
            }
        }

        if (0 == count) {
            if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev)) {
                perror("epoll_ctl del fd failure.");
                exit(EXIT_FAILURE);
            }
            close(fd);

            return 0;
        }

        printf("recv from client: %s\n", buf);
        struct echo_data* ed = (struct echo_data*)malloc(sizeof(struct echo_data));
        ed->data = buf;
        ed->fd = fd;
        ev.data.ptr = ed;
        ev.events = EPOLLOUT | EPOLLET;
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev)) {
            perror("epoll_ctl modify fd failure.");
            exit(EXIT_FAILURE);
        }

        return 0;
    } else if (ev.events & EPOLLOUT) {
        struct echo_data* data = (struct echo_data*)ev.data.ptr;
        printf("write data to client: %s", data->data);
        int ret = 0;
        int send_pos = 0;
        const int total = strlen(data->data);
        char* send_buf = data->data;
        while(1) {
            ret = write(data->fd, (send_buf + send_pos), total - send_pos);
            if (ret < 0) {
                if (EAGAIN == errno) {
                    sched_yield();
                    continue;
                }
                perror("write failure.");
                exit(EXIT_FAILURE);
            }
            send_pos += ret;
            if (total == send_pos) {
                break;
            }
        }

        ev.data.fd = data->fd;
        ev.events = EPOLLIN | EPOLLET;
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_MOD, data->fd, &ev)) {
            perror("epoll_ctl modify fd failure.");
            exit(EXIT_FAILURE);
        }

        free(data->data);
        free(data);
    }

    return 0;
}
