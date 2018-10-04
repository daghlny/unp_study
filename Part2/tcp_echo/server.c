
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "unp.h"

void
str_echo(int sockfd)
{
    ssize_t n;
    char buf[1024*1024];

    do{  
        while ((n = read(sockfd, buf, MAXLINE)) > 0) {
            printf("receive a message: ");
            fflush(stdout);
            write(fileno(stdout), buf, n);
            write(sockfd, buf, n);
        }
    } while (n<0 && errno == EINTR);
    if (n < 0)
        err_sys("str_echo: read error");
}

void
print_client_info(int sockfd)
{
    struct sockaddr_in sockadd;
    socklen_t socklen;
    if (getpeername(sockfd, (struct sockaddr*)&sockadd, &socklen) < 0)
        err_sys("getpeername error in print_client_info()");
    uint16_t port = ntohs(sockadd.sin_port);
    char* ipaddr_str = inet_ntoa(sockadd.sin_addr);

    printf("accept a client from %s with port %d\n", ipaddr_str, port);
}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        printf("%s portnumber\n", argv[0]);
        exit(-1);
    }
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        err_sys("bind error");

    if (listen(listenfd, LISTENQ) < 0)
        err_sys("listen error");

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        print_client_info(connfd);
        if ( (childpid = fork()) == 0 ) {
            close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        close(connfd);
    }

    return 0;
}
