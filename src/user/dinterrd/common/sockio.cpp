#include <cstring>
#include <iostream>

#include "sockio.h"
#include "server.h"
#include "sig.h"

void dinterr_readwait(dinterr_sock_t* dsock, char* buffer, size_t bsize, std::string* stream) {
    int readstat = SOCKIO_SUCCESS;
    *stream = "";

    do {
        readstat = dinterr_sock_read(dsock, buffer, bsize);
        stream->append((const char*) buffer, bsize);
    } while (readstat != SOCKIO_DONE);
}
        
void dinterr_sock_init(dinterr_sock_t* dsock, int type, const char* ipaddr) {
    memset(dsock, 0, sizeof(*dsock));
    dsock->type = type;

    if (type == DINTERR_SERVER)
        dsock->srv_sockfd = NOSOCKFD;
    else if (type == DINTERR_CLIENT)
        dsock->cli_sockfd = NOSOCKFD;

    dsock->conn_sockfd = NOSOCKFD;
    dsock->ipaddr = ipaddr;
    dsock->addrlen = sizeof(dsock->address);
    dsock->port = 0;
}

int dinterr_sock_create(dinterr_sock_t* dsock, uint16_t port, bool verbose) {
    int retval = SOCKIO_SUCCESS;
    int inet_aton_retval = 0;
    int sockerrno = 0;
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        retval = SOCKIO_FAIL;
        perror("dinterr_sock_create: socket()");
        return(retval);
    }

    dsock->address.sin_family = AF_INET;
    dsock->address.sin_port = htons(port);
    dsock->port = port;
    dsock->verbose = verbose;

    if (dsock->type == DINTERR_SERVER) {
        dsock->srv_sockfd = sockfd;
        dsock->address.sin_addr.s_addr = htonl(INADDR_ANY);

        retval = bind(dsock->srv_sockfd,
                      (struct sockaddr*)&dsock->address,
                      (socklen_t)dsock->addrlen);
        sockerrno = errno;

        if (retval == -1) {
            perror("dinterr_sock_create: bind()");
            goto sock_cleanup_fail;
        }

        if (dsock->verbose == true)
            std::cerr << "dinterrd: server bind() success" << std::endl;

        retval = listen(dsock->srv_sockfd, 0);
        sockerrno = errno;

        if (retval == -1) {
            perror("dinterr_sock_create: listen()");
            goto sock_cleanup_fail;
        }

        if (dsock->verbose == true) {
            std::cerr << "dinterrd: server listen() success" << std::endl;
            std::cerr << "dinterrd: server listening [" << port << "]" << std::endl;
        }
    }
    else if (dsock->type == DINTERR_CLIENT) {
        inet_aton_retval = inet_aton(dsock->ipaddr, &dsock->address.sin_addr);

        if (inet_aton_retval == -1) {
            perror("dinterr_sock_create: inet_aton()");
            goto sock_cleanup_fail;
        }

        dsock->cli_sockfd = sockfd;
        dsock->conn_sockfd = sockfd;
    }
    else {
        std::cerr << "dinterrd: invalid type specified." << std::endl;
        goto sock_cleanup_fail;
    }

    return(SOCKIO_SUCCESS);

sock_cleanup_fail:
    close(sockfd);
    dinterr_sock_init(dsock, dsock->type);
    return(SOCKIO_FAIL);
}

int dinterr_sock_read(dinterr_sock_t* dsock, char* buffer, size_t bsize) {
    int sockerr = 0;
    ssize_t readin = 1;
    int sockfd = NOSOCKFD;

    sockfd = dsock->conn_sockfd;
    if (sockfd == NOSOCKFD)
        return(SOCKIO_FAIL);

    memset(buffer, '\0', bsize);

try_sockread_again:
    readin = read(sockfd, buffer, bsize);
    sockerr = errno;

    if (readin == -1) {
        if (sockerr == EINTR)
            goto try_sockread_again;

        perror("dinterr_sock_read: read()");
        return(SOCKIO_FAIL);
    }

    if (readin <= bsize)
        return(SOCKIO_DONE);
    return(SOCKIO_MOREDATA);
}

int dinterr_sock_write(dinterr_sock_t* dsock, const char* buffer) {
    int sockioerrno = 0;
    ssize_t written = 0;
    size_t remaining = MAX_PAYLOAD_SIZE;
    int sockfd = NOSOCKFD;

    sockfd = dsock->conn_sockfd;
    if (sockfd == NOSOCKFD)
        return(SOCKIO_FAIL);

    while (remaining > 0) {
        size_t write_byte_count = SOCKIO_BYTE_COUNT(remaining);

        written = write(sockfd, buffer, write_byte_count);
        sockioerrno = errno;

        if (written == -1) {
            if (sockioerrno == EINTR)
                continue;

            perror("dinterr_sock_write");
            return(SOCKIO_FAIL);
        }

        remaining -= written;
        buffer += written;
    }

    return(SOCKIO_SUCCESS);
}

int dinterr_get_sockfd(dinterr_sock_t* dsock) {
    int type = NOSOCKFD;
    type = dsock->type;

    if (type == DINTERR_SERVER)
        return dsock->srv_sockfd;
    else if (type == DINTERR_CLIENT)
        return dsock->cli_sockfd;
    return NOSOCKFD;
}
