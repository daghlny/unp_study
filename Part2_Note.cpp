
struct in_addr{
    in_addr_t s_addr;
};

struct sockaddr_in {
    uint8_t         sin_len;
    sa_family_t     sin_family;     /* AF_INET */
    in_port_t       sin_port;
    struct in_addr  sin_addr;
    char            sin_zero[8];
};


#include <netinet/in.h>

/* 
 * h: host  n: network  s:short  l: long
 */
uint16_t htons(uint16_t host16bitvalue);
uint32_t htonl(uint32_t host32bitvalue);
uint16_t ntohs(uint16_t net16bitvalue);
uint32_t ntohl(uint32_t net32bitvalue);


#include <arpa/inet.h>

/*
 * p: presentation  n: numeric
 */
int         inet_pton(int family, const char* strptr, void* addrptr);
const char* inet_ntop(int family, const void* addrptr, char* strptr, size_t len); 

#include <sys/socket.h>

/*
 *                      AF_INET         AF_INET6        AF_LOCAL        AF_ROUTE        AF_KEY 
 * SOCK_STREAM         TCP | SCTP      TCP | SCTP        yes
 * SOCK_DGRAM             UDP             UDP            yes
 * SOCK_SEQPACKET         SCTP             SCTP          yes
 * SOCK_RAW               IPv4             IPv6                          yes             yes
 */

int socket(int family, int type, int protocol);

/*
 * return 0 for success, return -1 if error
 * if client doesn't receive SYN+ACK, errno = ETIMEDOUT
 * if client get a RST(hard error), errno = ECONNREFUSED
 * if client get a "destination unreachable"(soft error), errno = EHOSTUNREACH or ENETUNREACH
 */
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

/*
 * return 0 for success , return -1 if error
 * if address already in use, errno = EADDRINUSE
 */
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);

int listen(int sockfd, int backlog);

int accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);

#include <unistd.h>
int close(int sockfd);

#include <sys/socket.h>
/*
 * return 0 if success, -1 if error occurs
 */
int getsockname(int sockfd, struct sockaddr* localaddr, socklen_t* addrlen);
int getpeername(int sockfd, struct sockaddr* peeraddr, socklen_t* addrlen);

#include <sys/socket.h>

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
int setsockope(int sockfd, int level, int optname, const void* optval, socklen_t optlen);










