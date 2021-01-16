#include "server.h"

int dinterrd_processor_sm_wrapper(dinterr_sock_t* dsock, char* cli_addr, uint16_t src_port) {
    using namespace sml;
    sml::sm<ddtp_server> sm;
    return(_dinterrd_processor(dsock, &sm, cli_addr, src_port));
}

int _dinterrd_processor(dinterr_sock_t* dsock, sml::sm<ddtp_server>* sm, char* cli_addr, uint16_t src_port) {

    using namespace sml;
    int readstat = SOCKIO_SUCCESS;
    size_t bsize = sizeof(char) * SOCKIO_BUFFSIZE;
    char* buffer = (char*) malloc(bsize);
    std::string data_stream = "";

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
            dinterr_readwait(dsock, buffer, &data_stream);
            DinterrSerdesNetwork* sd = ddtp_serdes_create(data_stream.c_str());
            ddtp_payload_t* pl = (ddtp_payload_t*) sd->get_data();
            bool valid_type = ddtp_server_validate_type(pl->type, sm);

            if (dsock->verbose == true)
                printf("payload type: %d (0x%02X) validate: %d\n",
                       pl->type, pl->type, valid_type);

            if (valid_type == true)
                ddtp_server_process_incoming(pl, sm, dsock->verbose);

            ddtp_serdes_destroy(sd);

            std::cerr << "curr_state: terminal? (" << \
                sm->is(X) << ")" << std::endl;

            std::cerr << "curr_state: data_pend? (" << \
                sm->is("data_pend"_s) << ")" << std::endl;

            std::cerr << "curr_state: data_ready? (" << \
                sm->is("data_ready"_s) << ")" << std::endl;

            std::cerr << "curr_state: load_wait? (" << \
                sm->is("load_wait"_s) << ")" << std::endl;

            //break;   // TESTING: understood why based on netcat
                     // calls why this is needed for now. each
                     // nc all is a new client session which
                     // implies the sm object is always starting
                     // in intial state
        }

        free(buffer);
    }

    /*
    std::cerr << "event: *INIT*  curr_state: load_wait (" << \
      sm.is("load_wait"_s) << ")" << std::endl;

    sm.process_event(load_fail{});
    std::cerr << "event: load_fail  curr_state: load_wait (" << \
      sm.is("load_wait"_s) << ")" << std::endl;

    sm.process_event(load_success{});
    std::cerr << "event: load_success  curr_state: data_ready (" << \
      sm.is("data_ready"_s) << ")" << std::endl;

    sm.process_event(data_send{});
    std::cerr << "event: data_send  curr_state: data_ready (" << \
      sm.is("data_ready"_s) << ")" << std::endl;
    std::cerr << "event: data_send  curr_state: data_pend (" << \
      sm.is("data_pend"_s) << ")" << std::endl;
    */
    return(0);
}


bool ddtp_server_validate_type(short type, sml::sm<ddtp_server>* sm) {
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
            sm->process_event(terminate{});
    }

    return(validated);
}

/*
 * ddtp_server_process_incoming:
 *
 * the valid incoming payload types for the server are as follows:
 * - LOAD_REQUEST
 * - DATA_RETRY
 * - DATA_CONFIRM
 * - UNLOAD_REQUEST
 */
void ddtp_server_process_incoming(ddtp_payload_t* pl, sml::sm<ddtp_server>* sm, bool verbose) {
    using namespace sml;
    switch(pl->type) {
        case LOAD_REQUEST:
            std::cerr << "LOAD_REQUEST payload" << std::endl;
            // trigger load_request event to transition to
            // data_ready state (for now assume load_success{} event)
            /* try to load file for inotify from payload */
            sm->process_event(load_success{});

            /* now send inotify data to client, then
             * transition via data_send event to data_pend state
             */
            sm->process_event(data_send{});
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
