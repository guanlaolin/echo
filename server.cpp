#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const int SERVER_PORT = 8000;
const char SERVER_ADDR[] = "127.0.0.1";
const int MAX_CLIENT = 100;
const int MAX_SIZE = 1024;

int main()
{
    //definiton
    sockaddr_in server_addr,client_addr;

    //initialaziton
    bzero(&server_addr, sizeof(sockaddr_in));
    bzero(&client_addr, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
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

        socklen_t client_addr_size = sizeof(sockaddr);
        int client_sock = accept(sock, (sockaddr *)&client_addr, &client_addr_size);
        if ( -1 == client_sock )
        {
            perror("accept error");
            return -1;
        }
        printf("accept client:%s\n",inet_ntoa(client_addr.sin_addr));
        
        if (write( client_sock, "hello world", strlen("hello world")) == -1)
        {
            perror("write error");
        }

        char buff[MAX_SIZE];
        if (read( client_sock, buff, MAX_SIZE) == -1)
        {
            perror("read error");
            close(client_sock);
        }
        printf("read data:%s",buff);
        close(client_sock);

    close(sock);

    return 0;
}


