#include <iostream>
#include "thread.h"

#include "dataproto.cpp"

/*
 * Reference: https://man7.org/linux/man-pages/man7/inotify.7.html
 */
int _ddtp_load_file_inotify(ddtp_thread_data_t* tdat) {
    int fd;
    int* wd;
    pthread_t watchtid;

    if (tdat->payload->data != NULL) {
        fd = inotify_init1(IN_NONBLOCK);
        if (fd == -1) {
            perror("_ddtp_load_file_inotify: inotify_init1()");
            tdat->last_error = DDTP_LOAD_ERROR1;
            return DDTP_LOAD_ERROR1;
        }

        wd = (int*) calloc(1, sizeof(int));
        if (wd == NULL) {
            perror("_ddtp_load_file_inotify: calloc()");
            tdat->last_error = DDTP_LOAD_ERROR2;
            return DDTP_LOAD_ERROR2;
        }

        wd[0] = inotify_add_watch(fd, (const char*)tdat->payload->data, IN_ALL_EVENTS);
        if (wd[0] == -1) {
            perror("_ddtp_load_file_inotify: inotify_add_watch()");
            tdat->last_error = DDTP_LOAD_ERROR3;
            return DDTP_LOAD_ERROR3;
        }

        tdat->fd = fd;
        tdat->wd = wd;
    }
    else {
        _ddtp_char_verbot("_ddtp_load_file_inotify: empty payload field", tdat->sockfd->verbose);
        tdat->last_error = DDTP_LOAD_ERROR4;
        return DDTP_LOAD_ERROR4;
    }

    if (ddtp_increment_ref_count(tdat->locks) == 0) {
        if (pthread_create(&watchtid, NULL, _ddtp_watch_file_inotify, (void*) tdat) == 0) {
            pthread_detach(watchtid);
        }
        else {
            _ddtp_char_verbot("_ddtp_load_file_inotify: pthread_create() failed", tdat->sockfd->verbose);
            ddtp_decrement_ref_count(tdat->locks);
            tdat->last_error = DDTP_LOAD_ERROR6;
            return DDTP_LOAD_ERROR6;
        }
    }
    else {
        _ddtp_char_verbot("_ddtp_load_file_inotify: maximum references held", tdat->sockfd->verbose);
        tdat->last_error = DDTP_LOAD_ERROR5;
        return DDTP_LOAD_ERROR5;
    }

    return(0);
}

void _ddtp_process_client_payload(short type, ddtp_thread_data_t* tdat) {
    using namespace sml;
    sml::sm<ddtp_server>* sm = (sml::sm<ddtp_server>*) tdat->_sm;
    switch(type) {
        case LOAD_REQUEST:
            _ddtp_char_verbot("LOAD_REQUEST payload", tdat->sockfd->verbose);
            if (_ddtp_load_file_inotify(tdat) == 0)
                sm->process_event(load_success{});
            else
                sm->process_event(load_fail{});
            break;
    }
}

/*
 * the valid incoming payload types for the server are as follows:
 * - LOAD_REQUEST
 * - DATA_RETRY
 * - DATA_CONFIRM
 * - UNLOAD_REQUEST
 */
bool _ddtp_server_validate_incoming_type(short type, sml::sm<ddtp_server>* sm) {
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

void _ddtp_payload_md_verbot(short type, bool validated, bool verbose) {
    if (verbose == true) {
        printf("payload type: %d (0x%02X) validated?: %d\n", type, type, validated);
    }
}

void _ddtp_char_verbot(const char* msg, bool verbose) {
    if (verbose == true)
        std::cerr << msg << std::endl;
}

// Thread(s) and dependent functions

void* _ddtp_inotify_entry_point(void* data) {
    using namespace sml;
    ddtp_thread_data_t* _data = (ddtp_thread_data_t*) data;

    int readstat = SOCKIO_SUCCESS;
    size_t bsize = sizeof(char) * SOCKIO_BUFFSIZE;
    char* buffer = (char*) malloc(bsize);
    std::string data_stream = "";

    if (buffer != NULL) {
        /* while we haven't hit a terminal state
         * process socket io (sml::X) represents
         * the terminal state in the sml::sm object
         */
        bool verbose = _data->sockfd->verbose;
        sml::sm<ddtp_server>* sm = (sml::sm<ddtp_server>*) _data->_sm;

        while (sm->is(X) == false) {
            /* wait for something to come over the wire
             * and consume the stream ultimately as a
             * char array via the data_stream object.
             * this in turn will be deserialized into
             * a ddtp_payload_t object
             */
            if (sm->is("data_ready"_s) == true)
                ddtp_block_until_data_ready(_data->locks);
            dinterr_readwait(_data->sockfd, buffer, bsize, &data_stream);

            DinterrSerdesNetwork* sd = ddtp_serdes_create(data_stream.c_str());
            ddtp_payload_t* pl = (ddtp_payload_t*) sd->get_data();
            bool valid = _ddtp_server_validate_incoming_type(pl->type, sm);

            _ddtp_payload_md_verbot(pl->type, valid, verbose);
            _ddtp_state_verbot(sm, verbose);

            if (valid == true) {
                _data->payload = pl;
                _ddtp_process_client_payload(pl->type, _data);
            }

            ddtp_serdes_destroy(sd);
        }

        free(buffer);
    }
    else {
        _data->last_error = DDTP_ENTRY_ERROR1;
    }

    pthread_exit(NULL);
}

/*
 * core functional source code taken from inotify man page
 * for _server_load_file_and_watch() thread function
 *
 * Reference: https://man7.org/linux/man-pages/man7/inotify.7.html
 */
void* _ddtp_watch_file_inotify(void* data) {
    ddtp_thread_data_t* pl_data = (ddtp_thread_data_t*) data;
    int poll_num;
    int poll_err;
    nfds_t nfds = 1;
    struct pollfd fds[1];

    _ddtp_state_verbot((sml::sm<ddtp_server>*) pl_data->_sm, true);

    fds[0].fd = pl_data->fd;
    fds[0].events = POLLIN;

    while (true) {
        if (ddtp_get_ref_count(pl_data->locks) <= 0)
            break;
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("_server_inotify_file_watch: poll()");
            poll_err = DDTP_POLL_ERROR1;
            pthread_exit(NULL);
        }

        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                if (__handle_inotify_events(pl_data->fd,pl_data->wd, pl_data->data, pl_data->locks) != 0) {
                    poll_err = DDTP_EVENT_ERROR1;
                    pthread_exit(NULL);
                }
            }
        }
    }
    pthread_exit(NULL);
}

int __handle_inotify_events(int fd, int *wd, dinterr_crc32_data_table_t* dt, ddtp_locks_t* dlocks) {
    char buf[4096]
    __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;
    int eventerr = 0;

    for (;;) {
        eventerr = 0;
        len = read(fd, buf, sizeof(buf));

        if (len == -1 && errno != EAGAIN) {
            perror("__handle_inotify_events: read()");
            eventerr = DDTP_POLL_ERROR2;
            ddtp_decrement_ref_count(dlocks);
            break;
        }

        if (len <= 0)
            break;

        for (char *ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

            unsigned char attrs = 0;
            dinterr_ts_t timestamp;
            event = (const struct inotify_event *) ptr;

            /* INVESTIGATE:
             *   for some reason, getting events where
             *   pid is 0 and pos is 0. the crc value
             *   is 3330316196 for them. for now, we will
             *   do a pass on capturing these events, however,
             *   they could prove useful for understanding based
             *   on placement within other non-zero pid/pos events.
             *
             * UPDATE:
             *   because pid 0 is a reference to the kernel's
             *   scheduler, I am going to put it back in as an
             *   event to process (extra and redundant data)
             *
             *   Reference:
             *     https://en.wikipedia.org/wiki/Process_identifier
             */ 
            if (event->pos == 0 && event->pid == 0)
                continue;

            if (event->ra_page_count != 0 || event->ra_misses != 0)
                set_attr(&attrs, DINTERR_ATTR_READAHEAD);
            if (event->pos != event->last_cached_pos)
                set_attr(&attrs, DINTERR_ATTR_MULTISEEK);

            timestamp.tv_sec = event->fse_atime.tv_sec;
            timestamp.tv_usec = event->fse_atime.tv_usec;

            auto drecord = dinterrRecord()
                           .add_attrs(attrs)
                           .add_pos((dinterr_pos_t)event->pos)
                           .add_count((dinterr_count_t)event->count)
                           .add_pid((dinterr_pid_t)event->pid)
                           .add_ra_page_count((unsigned int)event->ra_page_count)
                           .add_ra_cache_misses((unsigned int)event->ra_misses)
                           .add_ra_last_cache_pos((dinterr_pos_t)event->last_cached_pos)
                           .add_timestamp(&timestamp)
                           .calc_crc();


            /* Testing...please remove the serdes bits once done...*/
            DinterrSerdesData* serdes = drecord.get_serdes();
            char* serial_data = (char*)serdes->get_data();

            DinterrSerdesData* unserdes = new DinterrSerdesData(serial_data);
            dinterr_data_t* unserial_data = (dinterr_data_t*)unserdes->get_data();
            std::cerr << "Pid: " << unserial_data->_pid << "  Pos: " << unserial_data->_pos << "  Crc32: " << unserial_data->_crc << std::endl;

            uLong crc32_key = drecord.get_crc();
            ddtp_lock(&dlocks->data_access_lock);

            dt->insert(
              crc_data_pair_t(
                crc32_key,
                ddtp_serdes_create((const char*)serdes->get_data())
              )
            );

            ddtp_signal_data_ready(dlocks);
            ddtp_unlock(&dlocks->data_access_lock);

            delete serdes;
        }
    }
    return(eventerr);
}
