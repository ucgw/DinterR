#ifndef _SOCKIO_H_
#define _SOCKIO_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DINTERR_SERVER 0
#define DINTERR_CLIENT 1

#define SOCKIO_RETRY_MAX 3
#define SOCKIO_RETRY_WAITTIME 1
#define SOCKIO_BUFFSIZE 4096

#define SOCKIO_FAIL -1
#define SOCKIO_SUCCESS 0
#define SOCKIO_MOREDATA 1
#define SOCKIO_DONE 2

#define SOCKIO_BYTE_COUNT(nbytes) (nbytes >= SOCKIO_MAX_BUFFSIZE ? \
                                   SOCKIO_BUFFSIZE : \
                                   nbytes)

typedef struct dinterr_sock {
    int type;
    union {
        int srv_sockfd;
        int cli_sockfd;
    };
    int addrlen;
    struct sockaddr_in address;
    const char* ipaddr;
    bool verbose;
    uint16_t port;
} dinterr_sock_t;

#endif // _SOCKIO_H_
