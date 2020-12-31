#ifndef _SOCKIO_H_
#define _SOCKIO_H_

#include <netinet/in.h>

#define DINTERR_SERVER 0
#define DINTERR_CLIENT 1

#define SOCKIO_BUFFSIZE 4096
#define SOCKIO_BYTE_COUNT(nbytes) (nbytes >= SOCKIO_MAX_BUFFSIZE ? \
                                   SOCKIO_BUFFSIZE : \
                                   nbytes)

typedef struct dinterr_sock {
    int type;
    int sockfd;
    int conn_sockfd;
    int addrlen;
    struct sockaddr_in address;
    const char* ipaddr;
    bool verbose;
    uint16_t port;
} dinterr_sock_t;

#endif // _SOCKIO_H_
