#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <time.h>

const int SERVER_PORT = 8000;
const char SERVER_ADDR[] = "127.0.0.1";
const int MAX_CLIENT = 100;
const int MAX_SIZE = 1024;

int main()
{
    //definiton
    int sock,client_sock;
    char buff[MAX_SIZE];
    sockaddr_in server_addr,client_addr;
    epoll_event event,events[MAX_CLIENT];

    //initialaziton
    bzero(&server_addr, sizeof(sockaddr_in));
    bzero(&client_addr, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( -1 == sock )
    {
        perror("socket error");
        return -1;
    }

    if (bind( sock, (sockaddr *)&server_addr, sizeof(sockaddr)) == -1)
    {
        perror("bind error");
        return -1;
    }

    if ( listen( sock, MAX_CLIENT) == -1)
    {
        perror("listen error");
        return -1;
    }

    //epoll begin
    //创建epoll
    int epfd = epoll_create1(0);
    if (-1 == epfd)
    {
        perror("epoll create");
        return -1;
    }

    //添加sock到epoll，监听连接事件
    bzero(&event, sizeof(epoll_event));
    event.events = EPOLLIN;
    event.data.fd = sock;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) == -1)
    {
        perror("epoll ctl");
        return -1;
    }

    for(;;)
    {
        int nfds = epoll_wait(epfd, events, MAX_CLIENT, -1);
        if (-1 == nfds)
        {
            perror("epoll wait");
            continue;
        }
        for( int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == sock)  //有连接
            {
                //accept
                socklen_t client_addr_size = sizeof(sockaddr);
                client_sock = accept(sock, (sockaddr *)&client_addr, &client_addr_size);
                if (-1 == client_sock)
                {
                    perror("accept error");
                    continue;
                }
                printf("accept client:%s\n",inet_ntoa(client_addr.sin_addr));
                
                //将client注册到epoll
                bzero(&event, sizeof(epoll_event));
                event.events = EPOLLIN;
                event.data.fd = client_sock;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event) == -1)
                {
                    perror("epoll ctl");
                    continue;
                }
            }else if (events[i].events & EPOLLIN)   //读事件
            {
                //读取数据
                if (read(events[i].data.fd, buff, MAX_SIZE) == -1)
                {
                    perror("read error");
                    continue;
                }
                printf("Read data from client:%s\n", buff);
                
                //设置为可写
                //将client注册到epoll
                bzero(&event, sizeof(epoll_event));
                event.events = EPOLLOUT;
                event.data.fd = events[i].data.fd;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1)
                {
                    perror("epoll ctl");
                    continue;
                }
            }else if (events[i].events & EPOLLOUT)  //写事件
            {
                //网客户端写入当前时间
                time_t t = time(NULL);
                if ( (time_t)-1 == t)
                {
                    perror("time");
                    continue;
                }
                printf("now time:%ld\n", t);
                
                if (write( events[i].data.fd, &t, sizeof(t)) == -1)
                {
                    perror("write error");
                    continue;
                }
                //设置为可读
                //将client注册到epoll
                bzero(&event, sizeof(epoll_event));
                event.events = EPOLLIN;
                event.data.fd = events[i].data.fd;
                if (epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1)
                {
                    perror("epoll ctl");
                    continue;
                }
            }else
            {
                printf("未实现的处理事件");
                continue;
            }
        }
    }
        
    close(epfd);
    close(client_sock);
    close(sock);

    return 0;
}


