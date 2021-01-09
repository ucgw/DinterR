#include <cstring>
#include <iostream>

#include "sockio.h"

void dinterr_sock_init(dinterr_sock_t* dsock, int type, const char* ipaddr) {
    memset(dsock, 0, sizeof(*dsock));
    dsock->type = type;

    if (type == DINTERR_SERVER)
        dsock->srv_sockfd = NOSOCKFD;
    else if (type == DINTERR_CLIENT)
        dsock->cli_sockfd = NOSOCKFD;

    dsock->ipaddr = ipaddr;
    dsock->addrlen = sizeof(dsock->address);
    dsock->verbose = true;
    dsock->port = 0;
}

int dinterr_sock_create(dinterr_sock_t* dsock, uint16_t port) {
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
