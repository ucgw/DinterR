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
    dinterr_crc32_data_table_t all_data;
    pthread_t entryid;

    ddtp_locks_init(&tlocks);

    tdat.last_error = 0;
    tdat.sockfd = dsock;
    tdat.payload = NULL;
    tdat.locks = &tlocks;
    tdat.data = &all_data;
    tdat._sm = (sml::sm<ddtp_server>*) sm;

    if (pthread_create(&entryid, NULL, _ddtp_inotify_entry_point, (void*)&tdat) == 0)
        pthread_join(entryid, NULL);
    else
        return DDTP_LOAD_ERROR6;

    return (int)tdat.last_error;
}
