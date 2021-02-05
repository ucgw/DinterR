#include "server.h"


int dinterrd_processor_sm_wrapper(dinterr_sock_t* dsock, char* cli_addr, uint16_t src_port) {
    using namespace sml;
    sml::sm<ddtp_server> sm;
    return(_dinterrd_processor(dsock, &sm, cli_addr, src_port));
}

int _dinterrd_processor(dinterr_sock_t* dsock, sml::sm<ddtp_server>* sm, char* cli_addr, uint16_t src_port) {
    using namespace sml;

    ddtp_thread_data_t tdat;
    ddtp_locks_t tlocks;
    pthread_t entryid;

    ddtp_locks_init(&tlocks);

    tdat.last_error = 0;
    tdat.sockfd = dsock;
    tdat.payload = NULL;
    tdat.locks = &tlocks;
    tdat._sm = (sml::sm<ddtp_server>*) sm;

    if (pthread_create(&entryid, NULL, _ddtp_inotify_entry_point, (void*)&tdat) == 0)
        pthread_join(entryid, NULL);
    else
        return DDTP_LOAD_ERROR6;

    return (int)tdat.last_error;
}

void dinterrd_run_server(dinterr_sock_t* dsock, uint16_t port, bool verbose, const char* ipaddr) {
    if (ipaddr == NULL)
        dinterr_sock_init(dsock, DINTERR_SERVER);
    else
        dinterr_sock_init(dsock, DINTERR_SERVER, ipaddr);

    if (dinterr_sock_create(dsock, port, verbose) == SOCKIO_SUCCESS)
        dinterrd_accept(dsock);
}

int dinterrd_accept(dinterr_sock_t* dsock) {
    if (dsock->type == DINTERR_SERVER) {
        int connsock = NOSOCKFD;

        while (true) {
            uint16_t src_port;
            char* cli_addr;
            connsock = accept(dsock->srv_sockfd,
                              (struct sockaddr*)&dsock->address,
                              (socklen_t*)&dsock->addrlen);

            if (connsock == -1) {
                perror("dinterrd_accept: accept()");
                return(SOCKIO_FAIL);
            }

            dsock->conn_sockfd = connsock;

            src_port = htons(dsock->address.sin_port);
            cli_addr = inet_ntoa(dsock->address.sin_addr);

            if (dsock->verbose == true)
                std::cerr << "dinterrd: client " << \
                             cli_addr << ":" << src_port << \
                             " connected" << std::endl;

            /* handle protocol / requests. 
             * for now, only one client
             * at a time can be handled
             */
            dinterrd_processor_sm_wrapper(dsock, cli_addr, src_port);

            close(connsock);
            if (dsock->verbose == true)
                std::cerr << "dinterrd: client " << \
                             cli_addr << ":" << src_port << \
                             " disconnected" << std::endl;
        }
        return(SOCKIO_SUCCESS);
    }
    return(SOCKIO_FAIL);
}
