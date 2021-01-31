#ifndef _SOCKIO_H_
#define _SOCKIO_H_

#include <cstddef>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "netproto.h"

#define NOSOCKFD -1

#define DINTERR_SERVER 0
#define DINTERR_CLIENT 1

#define SOCKIO_RETRY_MAX 3
#define SOCKIO_RETRY_WAITTIME 1
#define SOCKIO_BUFFSIZE MAX_PAYLOAD_SIZE

#define SOCKIO_FAIL -1
#define SOCKIO_SUCCESS 0
#define SOCKIO_MOREDATA 1
#define SOCKIO_DONE 2

#define SOCKIO_BYTE_COUNT(nbytes) (nbytes >= SOCKIO_BUFFSIZE ? \
                                   SOCKIO_BUFFSIZE : \
                                   nbytes)

typedef struct dinterr_sock {
    // either DINTERR_SERVER or DINTERR_CLIENT
    int type;
    union {
        int srv_sockfd;
        int cli_sockfd;
    };
    int conn_sockfd;
    int addrlen;
    struct sockaddr_in address;
    const char* ipaddr;
    bool verbose;
    uint16_t port;
} dinterr_sock_t;

// helper functions to setup appropriate socket types
// for either server or client
void dinterr_sock_init(dinterr_sock_t*, int, const char* ipaddr=NULL);
int dinterr_sock_create(dinterr_sock_t*, uint16_t, bool verbose=false);
int dinterr_get_sockfd(dinterr_sock_t*);

// functions for server and client to accept and connect
// for service
//
// moved dinterrd_accept() to server.h due to dependency
// issue with client needing server.o linkage when declared
// and defined in sockio.h
// -gw 01/24

// moved dinterrd_run_server() to server.h for reasons
// similar to dinterrd_accept()
// -gw 01/24


// basic data read / write over sockets
int dinterr_sock_read(dinterr_sock_t*, char*, size_t);
int dinterr_sock_write(dinterr_sock_t*, const char*);

void dinterr_readwait(dinterr_sock_t*, char*, size_t, std::string*);
#endif // _SOCKIO_H_
