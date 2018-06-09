
#include <vector>
#include <algorithm>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "unp.h"

using std::vector;

void
str_echo(int sockfd)
{
    ssize_t n;
    char buf[1024*1024];

    do{  
        while ((n = read(sockfd, buf, MAXLINE)) > 0) {
            printf("[thread %d]receive a message: ", (int)pthread_self());
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

    printf("[thread %d]accept a client from %s with port %d\n", (int)pthread_self(), ipaddr_str, port);
}

void* thread_fn(void* arg)
{
    int self_id = int(pthread_self());
    printf("BEGIN creating thread %d\n", self_id);
    struct sockaddr_in* addr = (struct sockaddr_in*)arg;

    int listenfd;
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Thread(%d): ", self_id);
        err_sys("socket create error");
    }

    /* set the SO_REUSEPORT option */
    int reuseport_opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (void*)(&reuseport_opt), sizeof(int)) < 0){
        printf("Thread(%d): ", self_id);
        err_sys("setsockopt error");
    }
    
    if (bind(listenfd, (struct sockaddr*)addr, sizeof(struct sockaddr_in)) < 0){
        printf("Thread(%d): ", self_id);
        err_sys("bind error");
    }

    if (listen(listenfd, 10) < 0) {
        printf("Thread(%d): ", self_id);
        err_sys("listen error");
    }

    int maxfdp1;
    fd_set rset;
    struct sockaddr_in client_addr;
    char buf[1024*1024];
    vector<int> fds;
    fds.push_back(listenfd);

    for (;;) {

        maxfdp1 = 0;
        for (auto fd : fds) {
            maxfdp1 = std::max(maxfdp1, fd);
            FD_SET(fd, &rset);
        }
        maxfdp1 += 1;

        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset)) {
            socklen_t client_len;
            int connfd;
            if ( (connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_len)) < 0){
                printf("Thread(%d): ", self_id);
                err_sys("accept error");
            }
            fds.push_back(connfd);
            print_client_info(connfd);
        }

        for (auto fd : fds) {
            if (fd == listenfd) continue;
            if (FD_ISSET(fd, &rset))
                str_echo(fd);
        }
    }

    return NULL;
}

int
main(int argc, char **argv)
{
    if (argc < 3) {
        printf("%s <portnumber> <threadnum>\n", argv[0]);
        exit(-1);
    }

    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    int threadnum = atoi(argv[2]);
    vector<pthread_t> threads(threadnum);
    for (int i = 0; i < threadnum; ++i) {
        pthread_create(&threads[i], NULL, thread_fn, (void*)(&servaddr));
    }

    for (int i = 0; i < threadnum; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

