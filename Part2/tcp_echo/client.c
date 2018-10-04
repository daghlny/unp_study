

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
/*
void
str_cli(FILE* fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    while ( fgets(sendline, MAXLINE, fp) != NULL ) 
    {
        write(sockfd, sendline, strlen(sendline));
        if (read(sockfd, recvline, MAXLINE) == 0)
            err_sys("str_cli: server terminated prematurely");
        fputs(recvline, stdout);
    }
}
*/

/* using select */
/*
void 
str_cli(FILE* fp, int sockfd) 
{
    int maxfdp1;
    fd_set rset;
    char sendline[MAXLINE], recvline[MAXLINE];

    FD_ZERO(&rset);
    for (;;)
    {
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        maxfdp1 = (fileno(fp) > sockfd ? fileno(fp) : sockfd) + 1;
        
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            // socket is readable
            if (read(sockfd, recvline, MAXLINE) == 0)
                err_sys("str_cli: server terminated prematurely");
            fputs(recvline, stdout);
        } 

        if (FD_ISSET(fileno(fp), &rset)) {
            // input is readable
            if ( fgets(sendline, MAXLINE, fp) == NULL )
                return ;
            write(sockfd, sendline, strlen(sendline));
        }
    }
}
*/

/* version 3 in chapter 6.7 */
void
str_cli(FILE *fp, int sockfd)
{
    int maxfdp1, stdineof;

    fd_set rset;
    char buf[MAXLINE];

    int n;
    
    stdineof = 0;
    FD_ZERO(&rset);

    for (;;) {
        if (stdineof == 0)
            FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);

        maxfdp1 = ( fileno(fp) > sockfd ? fileno(fp) : sockfd ) + 1;
        select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &rset)) {
            if ( ( n = read(sockfd, buf, MAXLINE) ) == 0 ) {
                if (stdineof == 1)
                    return ;
                else
                    err_sys("str_cli: server terminated prematurely");
            }
            write(fileno(stdout), buf, n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if ( (n = read(fileno(fp), buf, MAXLINE)) == 0 ) {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;
            }
            write(sockfd, buf, n);
        }
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
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) 
        err_sys("connect error");

    str_cli(stdin, sockfd);

    return 0;
}

