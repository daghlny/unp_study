
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
str_echo(int sockfd, struct sockaddr* pcliaddr, socklen_t clilen)
{
    ssize_t n;
    char buf[1024*1024];
    socklen_t len;

    for (;;) {
        len = clilen;
        n = recvfrom(sockfd, buf, sizeof(buf), 0, pcliaddr, &len);
        printf("receive message: %s\n", buf);
        sendto(sockfd, buf, n, 0, pcliaddr, len);
    }

}

int
main(int argc, char **argv)
{
    if (argc < 2) {
        printf("%s portnumber\n", argv[0]);
        exit(-1);
    }

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    bind(sockfd, (struct sockaddr_in*)&servaddr, sizeof(servaddr));

    str_echo(sockfd, (struct sockaddr_in*)&cliaddr, sizeof(cliaddr));


    return 0;
}
