

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "unp.h"

/* version 3 in chapter 6.7 */
void
str_cli(FILE *fp, int sockfd, struct sockaddr_in *addr)
{
    char buf[1024*1024];
    char readbuf[1024*1024];
    for (;;) {
        memset(buf, 0, sizeof(buf));
        memset(readbuf, 0, sizeof(readbuf));
        read(fileno(fp), buf, sizeof(buf));
        socklen_t serv_len;
        sendto(sockfd, (void*)buf, strlen(buf), 0, (struct sockaddr*)addr, (socklen_t)(sizeof(struct sockaddr_in)));
        recvfrom(sockfd, (void*)readbuf, sizeof(readbuf), 0, addr, &serv_len);
        printf("%s\n", readbuf);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 3) {
        printf("%s <IPaddress> <Port>\n", argv[0]);
        exit(-1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    str_cli(stdin, sockfd, &servaddr);

    return 0;
}

