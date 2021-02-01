#include <iostream>
#include <cstring>

#include "thread.h"
#include "payload.h"
#include "queue.h"

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
    ddtp_payload_t clipl;
    short error_code;

    switch(type) {
        case LOAD_REQUEST:
            _ddtp_char_verbot("LOAD_REQUEST payload", tdat->sockfd->verbose);
            error_code = _ddtp_load_file_inotify(tdat);
            if (error_code == 0) {
                ddtp_payload_init(LOAD_SUCCEED, &clipl);
                _ddtp_server_send_payload(&clipl, tdat->sockfd);

                ddtp_lock(&tdat->locks->sm_lock);
                sm->process_event(load_success{});
                ddtp_unlock(&tdat->locks->sm_lock);
            }
            else {
                ddtp_payload_init(LOAD_FAIL, &clipl);
                ddtp_payload_fill_short_data(&clipl, error_code);
                _ddtp_server_send_payload(&clipl, tdat->sockfd);

                ddtp_lock(&tdat->locks->sm_lock);
                sm->process_event(load_fail{});
                ddtp_unlock(&tdat->locks->sm_lock);
            }
            break;
    }
}

/*
 * the valid incoming payload types for the server are as follows:
 * - LOAD_REQUEST
 * - UNLOAD_REQUEST
 */
bool _ddtp_server_validate_incoming_type(short type, ddtp_thread_data_t* tdat) {
    bool validated = false;

    using namespace sml;
    sml::sm<ddtp_server>* sm = (sml::sm<ddtp_server>*) tdat->_sm;

    switch(type) {
        case LOAD_REQUEST:
            if (sm->is("load_wait"_s))
                validated = true;
            break;
        case UNLOAD_REQUEST:
            if (sm->is("data_ready"_s))
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
    if (validated == false) {
        ddtp_lock(&tdat->locks->sm_lock);
        sm->process_event(terminate{});
        ddtp_unlock(&tdat->locks->sm_lock);
    }

    return(validated);
}

// the ddtp state verbose bot
void _ddtp_state_verbot(sml::sm<ddtp_server>* sm, ddtp_locks_t* locks, bool verbose) {
    bool terminal_state_teller;
    bool data_ready_state_teller;
    bool load_wait_state_teller;
    bool unload_pend_state_teller;

    using namespace sml;
    if (verbose == true) {
        ddtp_lock(&locks->sm_lock);
        terminal_state_teller = sm->is(X);
        data_ready_state_teller = sm->is("data_ready"_s);
        load_wait_state_teller = sm->is("load_wait"_s);
        unload_pend_state_teller = sm->is("unload_pend"_s);
        ddtp_unlock(&locks->sm_lock);
        
        std::cerr << \
            "CURR_STATE: terminal:" << \
            terminal_state_teller << "  " << \
            "data_ready:" << \
            data_ready_state_teller << "  " << \
            "load_wait:" << \
            load_wait_state_teller << "  " << \
            "unload_pend:" << \
            unload_pend_state_teller << \
        std::endl;
    }
}

void _ddtp_payload_md_verbot(short type, bool validated, bool verbose) {
    if (verbose == true)
        printf("payload type: %d (0x%02X) validated?: %d\n", type, type, validated);
}

void _ddtp_char_verbot(const char* msg, bool verbose) {
    if (verbose == true)
        std::cerr << msg << std::endl;
}

void _ddtp_data_client_target_verbot(crc_data_pair_t* pl_data, bool verbose) {
    if (verbose == true) {
        int i = 0;
        uLong crc = std::get<0>(*pl_data);
        DinterrSerdesData serdes = std::get<1>(*pl_data);
        const char* data = (const char*) serdes.get_data();

        printf("data crc32: %lu\n", crc);
        for (; i <= sizeof(dinterr_data_t); i++) {
            printf("0x%02X ", data[i]);
        }
        printf("\n----------\n");

        DinterrSerdesData* unserdes = new DinterrSerdesData(data);
        dinterr_data_t* d_data = (dinterr_data_t*)unserdes->get_data();

        std::cout << "CRC32: " << d_data->_crc << " PID: " << d_data->_pid << " POS: " << d_data->_pos << std::endl;
        printf("\n----------\n");

        delete unserdes;
    }
}

int _ddtp_send_data_client_target(crc_data_pair_t* _data, dinterr_sock_t* dsock) {
    uLong crc = std::get<0>(*_data);
    DinterrSerdesData serdes = std::get<1>(*_data);
    const char* data = (const char*) serdes.get_data();

    return(dinterr_sock_write(dsock, data));
}

/*
 * Analog: ddtp_client_send_payload()
 */
int _ddtp_server_send_payload(ddtp_payload_t* pl, dinterr_sock_t* dsock) {
    DinterrSerdesNetwork* srv_pl = new DinterrSerdesNetwork(pl);
    char* pl_data = (char*) srv_pl->get_data();
    int retval = dinterr_sock_write(dsock, (const char*) pl_data);
    delete srv_pl;
    return(retval);
}

// Thread(s) and dependent functions

void* _ddtp_inotify_entry_point(void* data) {
    using namespace sml;
    ddtp_thread_data_t* _data = (ddtp_thread_data_t*) data;

    size_t bsize = MAX_PAYLOAD_SIZE;
    char* readin_buffer = (char*) malloc(bsize);
    std::string data_stream = "";
    ddtp_payload_t clipl;
    pthread_t asyncplid;
    bool terminal_state_teller;
    bool data_ready_state_teller;
    bool load_wait_state_teller;
    bool unload_pend_state_teller;

    if (readin_buffer != NULL) {
        bool verbose = _data->sockfd->verbose;
        sml::sm<ddtp_server>* sm = (sml::sm<ddtp_server>*) _data->_sm;

        /*   spawn another thread for handling incoming
         *   UNLOAD_REQUEST from client. this thread
         *   will have the capability to signal data_ready
         *   but before doing so, will transition to
         *   unload_pend state, which will prohibit entering
         *   this code path next iteration of the while()
         */
        if (pthread_create(&asyncplid, NULL, _ddtp_async_client_payload_handler, (void*) _data) == 0) {
            pthread_detach(asyncplid);
        }
        else {
            _data->last_error = DDTP_LOAD_ERROR6;
            pthread_exit(NULL);
        }

        /* while we haven't hit a terminal state
         * process socket io (sml::X) represents
         * the terminal state in the sml::sm object
         */
        terminal_state_teller = false;
        while (terminal_state_teller == false) {
            /* wait for something to come over the wire
             * and consume the stream ultimately as a
             * char array via the data_stream object.
             * this in turn will be deserialized into
             * a ddtp_payload_t object
             */
            ddtp_lock(&_data->locks->sm_lock);
            data_ready_state_teller = sm->is("data_ready"_s);
            load_wait_state_teller = sm->is("load_wait"_s);
            unload_pend_state_teller = sm->is("unload_pend"_s);
            ddtp_unlock(&_data->locks->sm_lock);

            if (data_ready_state_teller == true) {
                /* conditional lock on having full records in the
                 * global queue to send to our client, this blocks
                 * until data_ready_lock is set to DDTP_DATA_READY
                 */
                ddtp_block_until_data_ready(_data->locks);

                while (auto t = gq.pop()) {
                    _ddtp_data_client_target_verbot((crc_data_pair_t*)&t, _data->sockfd->verbose);
                    _ddtp_send_data_client_target((crc_data_pair_t*)&t, _data->sockfd);
                }
            }
            else if (load_wait_state_teller == true) {
                dinterr_readwait(_data->sockfd, readin_buffer, bsize, &data_stream);
                DinterrSerdesNetwork* sd = ddtp_serdes_create(data_stream.c_str());
                ddtp_payload_t* pl = (ddtp_payload_t*) sd->get_data();
                bool valid = _ddtp_server_validate_incoming_type(pl->type, _data);

                _ddtp_payload_md_verbot(pl->type, valid, verbose);

                if (valid == true) {
                    _data->payload = pl;
                    _ddtp_process_client_payload(pl->type, _data);
                }

                ddtp_serdes_destroy(sd);
                memset(readin_buffer, '\0', bsize);
            }
            else if (unload_pend_state_teller == true) {
                /* client has sent an UNLOAD_REQUEST signaling to
                 * server that they wish to finish session, so process
                 * any remaining records and send to client before
                 * entering into terminal state effectively disconnecting
                 * client.
                 */
                ddtp_lock(&_data->locks->sm_lock);
                sm->process_event(terminate{});
                ddtp_unlock(&_data->locks->sm_lock);
            }

            ddtp_lock(&_data->locks->sm_lock);
            terminal_state_teller = sm->is(X);
            ddtp_unlock(&_data->locks->sm_lock);

            _ddtp_state_verbot(sm, _data->locks, verbose);
        }

        free(readin_buffer);
    }
    else {
        _data->last_error = DDTP_ENTRY_ERROR1;
    }

    pthread_exit(NULL);
}

void* _ddtp_async_client_payload_handler(void* data) {
    using namespace sml;
    ddtp_thread_data_t* _data = (ddtp_thread_data_t*) data;
    sml::sm<ddtp_server>* sm = (sml::sm<ddtp_server>*) _data->_sm;

    size_t bsize = MAX_PAYLOAD_SIZE;
    char* readin_buffer = (char*) malloc(bsize);
    std::string data_stream = "";
    DinterrSerdesNetwork* sd;
    ddtp_payload_t* pl;
    bool valid;

    while (true) {
        dinterr_readwait(_data->sockfd, readin_buffer, bsize, &data_stream);
        sd = ddtp_serdes_create(data_stream.c_str());
        pl = (ddtp_payload_t*) sd->get_data();
        valid = _ddtp_server_validate_incoming_type(pl->type, _data);

        if (valid == true && pl->type == UNLOAD_REQUEST) {
            ddtp_lock(&_data->locks->sm_lock);
            sm->process_event(unload_request{});
            ddtp_unlock(&_data->locks->sm_lock);

            goto free_signal;
        }
            
        ddtp_serdes_destroy(sd);
        memset(readin_buffer, '\0', bsize);
    }

free_signal:
    free(readin_buffer);
    ddtp_serdes_destroy(sd);
    ddtp_signal_data_ready(_data->locks);
    pthread_exit(NULL);
}

/*
 * core functional source code taken from inotify man page
 * for _server_load_file_and_watch() thread function
 *
 * Reference: https://man7.org/linux/man-pages/man7/inotify.7.html
 */
void* _ddtp_watch_file_inotify(void* data) {
    ddtp_thread_data_t* _data = (ddtp_thread_data_t*) data;
    int poll_num;
    nfds_t nfds = 1;
    struct pollfd fds[1];
    sml::sm<ddtp_server>* sm = (sml::sm<ddtp_server>*) _data->_sm;

    _ddtp_state_verbot(sm, _data->locks, true);

    fds[0].fd = _data->fd;
    fds[0].events = POLLIN;

    while (true) {
        if (ddtp_get_ref_count(_data->locks) <= 0)
            break;
        if (fds[0].fd < 0)
            break;
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("_server_inotify_file_watch: poll()");
            _data->last_error = DDTP_POLL_ERROR1;
            break;
        }

        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                if (__handle_inotify_events(_data->fd, _data->wd, _data->locks) != 0) {
                    _data->last_error = DDTP_EVENT_ERROR1;
                    break;
                }

            }
        }

        if (gq.size() > 0)
            ddtp_signal_data_ready(_data->locks);
        else
            break;
    }
    pthread_exit(NULL);
}

int __handle_inotify_events(int fd, int *wd, ddtp_locks_t* dlocks) {
    char buf[4096]
    __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;
    int eventerr = 0;

    for (;;) {
        eventerr = 0;
        if (fd < 0)
            return(DDTP_POLL_ERROR2);

        len = read(fd, buf, sizeof(buf));

        if (len == -1 && errno != EAGAIN) {
            perror("__handle_inotify_events: read()");
            eventerr = DDTP_POLL_ERROR2;
            ddtp_decrement_ref_count(dlocks);
            return(eventerr);
        }

        if (len <= 0)
            break;

        for (char *ptr = buf; ptr < buf + len;
             ptr += sizeof(struct inotify_event) + event->len) {

            unsigned char attrs = 0;
            dinterr_ts_t timestamp;
            event = (const struct inotify_event *) ptr;

            /* 
             * because pid 0 is a reference to the kernel's
             * scheduler, I am going to put it back in as an
             * event to filter (extra and redundant data)
             *
             * Reference:
             *   https://en.wikipedia.org/wiki/Process_identifier
             */ 
            if (event->pid == 0)
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


            uLong crc32_key = drecord.get_crc();

            DinterrSerdesData* serdes = drecord.get_serdes();
            DinterrSerdesData sd_copy = *serdes;

            crc_data_pair_t d_pair (crc32_key, sd_copy);

            gq.push(d_pair);

            delete serdes;
        }
    }

    return(eventerr);
}
