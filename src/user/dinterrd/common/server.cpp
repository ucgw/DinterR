#include "server.h"


int dinterrd_processor_sm_wrapper(dinterr_sock_t* dsock, char* cli_addr, uint16_t src_port) {
    using namespace sml;
    sml::sm<ddtp_server> sm;
    return(_dinterrd_processor(dsock, &sm, cli_addr, src_port));
}

// the ddtp state verbose bot
void _ddtp_state_verbot(sml::sm<ddtp_server>* sm, bool verbose) {
    using namespace sml;
    if (verbose == true) {
        std::cerr << "CURR_STATE: terminal:" << sm->is(X) << \
              "  " << "data_pend:" << sm->is("data_pend"_s) << \
              "  " << "data_ready:" << sm->is("data_ready"_s) << \
              "  " << "data_purge:" << sm->is("data_purge"_s) << \
              "  " << "load_wait:" << sm->is("load_wait"_s) << \
              "  " << "unload_pend:" << sm->is("unload_pend"_s) << \
        std::endl;
    }
}

int _dinterrd_processor(dinterr_sock_t* dsock, sml::sm<ddtp_server>* sm, char* cli_addr, uint16_t src_port) {

    using namespace sml;
    int readstat = SOCKIO_SUCCESS;
    size_t bsize = sizeof(char) * SOCKIO_BUFFSIZE;
    char* buffer = (char*) malloc(bsize);
    std::string data_stream = "";
    ddtp_thread_data_t tdat;
    ddtp_locks_t tlocks;
    dinterr_crc32_data_table_t all_data;

    ddtp_locks_init(&tlocks);

    tdat.sockfd = dsock;
    tdat.payload = NULL;
    tdat.locks = &tlocks;
    tdat.data = &all_data;

    if (buffer != NULL) {
        /* while we haven't hit a terminal state
         * process socket io (sml::X) represents
         * the terminal state in the sml::sm object
         */
        while (sm->is(X) == false) {
           /* wait for something to come over the wire
            * and consume the stream ultimately as a
            * char array via the data_stream object.
            * this in turn will be deserialized into
            * a ddtp_payload_t object
            */
            dinterr_readwait(dsock, buffer, bsize, &data_stream);
            std::cerr << "**> " << data_stream << " <**" << std::endl;
            DinterrSerdesNetwork* sd = ddtp_serdes_create(data_stream.c_str());
            ddtp_payload_t* pl = (ddtp_payload_t*) sd->get_data();
            bool valid_type = ddtp_server_validate_incoming_type(pl->type, sm);

            if (dsock->verbose == true) {
                printf("payload type: %d (0x%02X) validated: %d\n",
                       pl->type, pl->type, valid_type);
                _ddtp_state_verbot(sm, true);
            }

            if (valid_type == true) {
                tdat.payload = pl;
                std::cerr << "==> " << ddtpPayloadType[pl->type] << " <==" << std::endl;
                std::cerr << "==> " << pl->data << " <==" << std::endl;
                ddtp_server_process_incoming_payload(&tdat, sm);
            }

            ddtp_serdes_destroy(sd);

            break;   // TESTING: understood why based on netcat
                     // calls why this is needed for now. each
                     // nc all is a new client session which
                     // implies the sm object is always starting
                     // in intial state
        }

        free(buffer);
    }

    return(0);
}

/*
 * the valid incoming payload types for the server are as follows:
 * - LOAD_REQUEST
 * - DATA_RETRY
 * - DATA_CONFIRM
 * - UNLOAD_REQUEST
 */

bool ddtp_server_validate_incoming_type(short type, sml::sm<ddtp_server>* sm) {
    bool validated = false;

    using namespace sml;
    switch(type) {
        case LOAD_REQUEST:
            if (sm->is("load_wait"_s))
                validated = true;
            break;
        case DATA_RETRY:
            if (sm->is("data_pend"_s))
                validated = true;
            break;
        case DATA_CONFIRM:
            if (sm->is("data_pend"_s))
                validated = true;
            break;
        case UNLOAD_REQUEST:
            if (sm->is("data_pend"_s))
                validated = true;
            break;
        default:
            // do nothing, validated is false
            // by default
            ;
    }

    // transition to terminal state
    // if we haven't positively validated type
    // relative to expected state
    if (validated == false)
        sm->process_event(terminate{});

    return(validated);
}

void ddtp_server_process_incoming_payload(ddtp_thread_data_t* tdat, sml::sm<ddtp_server>* sm) {
    using namespace sml;
    switch(tdat->payload->type) {
        case LOAD_REQUEST:
            std::cerr << "LOAD_REQUEST payload" << std::endl;
            // trigger load_request event to transition to
            // data_ready state (for now assume load_success{} event)
            /* try to load file for inotify from payload */
            if (ddtp_server_load_file_inotify(tdat) == 0)
                sm->process_event(load_success{});
            else
                sm->process_event(load_fail{});
            break;
        case DATA_RETRY:
            std::cerr << "LOAD_RETRY payload" << std::endl;
            sm->process_event(data_retry_request{});

            /* now resend inotify data to client, then
             * transition via data_send event to data_pend state
             */
            sm->process_event(data_send{});
            break;
        case DATA_CONFIRM:
            std::cerr << "DATA_CONFIRM payload" << std::endl;
            sm->process_event(data_verify_confirm{});

            /* now purge data (internal), then transition
             * via data_purge event to data_ready state
             */
            sm->process_event(data_purge{});
            break;
        case UNLOAD_REQUEST:
            std::cerr << "UNLOAD_REQUEST payload" << std::endl;
            // terminal state transition
            // (assumes we can transition here from current state)
            sm->process_event(terminate{});
            break;
        default:
            std::cerr << "*- Error: INVALID PAYLOAD -*" << std::endl;
            break;
    }
}

/*
 * Reference: https://man7.org/linux/man-pages/man7/inotify.7.html
 */
int ddtp_server_load_file_inotify(ddtp_thread_data_t* tdat) {
    int fd;
    int* wd;
    pthread_t loadtid;

    if (tdat->payload->data != NULL) {
        fd = inotify_init1(IN_NONBLOCK);
        if (fd == -1) {
            perror("ddtp_server_load_file_inotify: inotify_init1()");
            return DDTP_LOAD_ERROR1;
        }

        wd = (int*) calloc(1, sizeof(int));
        if (wd == NULL) {
            perror("ddtp_server_load_file_inotify: calloc()");
            return DDTP_LOAD_ERROR2;
        }

        wd[0] = inotify_add_watch(fd, (const char*) tdat->payload->data, IN_ALL_EVENTS);
        if (wd[0] == -1) {
            perror("ddtp_server_load_file_inotify: inotify_add_watch()");
            return DDTP_LOAD_ERROR3;
        }
    }
    else {
        std::cerr << "ddtp_server_load_file_inotify: payload field is empty" << std::endl;
        return DDTP_LOAD_ERROR4;
    }

    /* FIXME: move from double locking to single locking to do
     *        do the right thing here.
     */
    if (ddtp_get_ref_count(tdat->locks) < DDTP_MAX_REFERENCES) {
        // at this point we are committing to inotify events
        // for the filename passed in from the client, so
        // increment our reference counter 
        ddtp_increment_ref_count(tdat->locks);

        // create thread and detach for the inotify event handler
        if (pthread_create(&loadtid, NULL, _server_inotify_file_watch, (void*) tdat) == 0)
            pthread_detach(loadtid);
        else {
            std::cerr << "ddtp_server_load_file_inotify: pthread_create() failed" << std::endl;
            ddtp_decrement_ref_count(tdat->locks);
            return DDTP_LOAD_ERROR6;
        }
    }
    else {
        std::cerr << "ddtp_server_load_file_inotify: maximum references held" << std::endl;
        return DDTP_LOAD_ERROR5;
    }

    return(0);
}
