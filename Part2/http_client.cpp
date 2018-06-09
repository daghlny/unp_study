
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
#include <fcntl.h>

#include "unp.h"

void
str_cli(FILE* fp, int sockfd)
{
    char sendline[MAXLINE], recvline[1024*1024];
    if (read(fileno(fp), sendline, MAXLINE) <= 0)
        err_sys("str_cli: read error");
    if (write(sockfd, sendline, strlen(sendline)) <= 0)
        err_sys("str_cli: write error");

    sleep(1);

    int read_ret = 0;
    while ( (read_ret = read(sockfd, recvline, MAXLINE)) > 0) {
        write(fileno(stdout), recvline, strlen(recvline));
        memset(recvline, 0, sizeof(recvline));
    }

    if (read_ret == 0)
        err_sys("str_cli: read the EOF");
    else if (read_ret == -1 && errno != EAGAIN)
        err_sys("str_cli: read error");
    else if (errno == EAGAIN)
        err_sys("str_cli: EAGAIN");



    return ;
}

int 
main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 4) {
        printf("%s <IPaddress> <Port> <HTTP_header_file>\n", argv[0]);
        exit(-1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    FILE* http_header_file = fopen(argv[3], "r");
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if ( (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) 
        err_sys("connect error");

    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    str_cli(http_header_file, sockfd);

    return 0;
}

