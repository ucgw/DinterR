#include "client.h"

int dinterrd_run_client(dinterr_sock_t* dsock, int iterations, uint16_t port, const char* ipaddr, const char* filename, const char* csvfile, bool verbose) {
    using namespace sml;
    sml::sm<ddtp_client> sm;
    std::ofstream csv;

    if (open_for_append(csvfile, &csv) == false)
        return(SOCKIO_FAIL);

    dinterr_sock_init(dsock, DINTERR_CLIENT, ipaddr);
    int retval = dinterr_sock_create(dsock, port, verbose);

    if (retval == SOCKIO_SUCCESS)
        return(dinterrd_connect(dsock, iterations, &sm, filename, &csv));
    csv.close();
    return(retval);
}

int dinterrd_connect(dinterr_sock_t* dsock, int iterations, sml::sm<ddtp_client>* sm, const char* filename, std::ofstream* csv) {
    using namespace sml;
    struct pollfd poll_sockfd[1];
    int connsock = NOSOCKFD;
    int connect_retval = 0;
    char* response = NULL;
    ddtp_payload_t pl;
    size_t bsize = MAX_PAYLOAD_SIZE;
    std::string data_stream = "";
    short error_code;
    int iter_count = 0;

    if (dsock->type == DINTERR_CLIENT) {
        connect_retval = connect(dsock->cli_sockfd,
                           (struct sockaddr*)&dsock->address,
                           (socklen_t)dsock->addrlen);

        if (connect_retval == -1) {
            perror("dinterrd_connect: connect()");
            goto free_and_fail;
        }

        response = (char*)malloc(sizeof(char) * bsize);

        if (response == NULL) {
            shutdown(dsock->cli_sockfd, SHUT_RDWR);
            std::cerr << "dinterrd_connect: response buffer malloc() failed" << std::endl;
            goto free_and_fail;
        }

        if (dsock->verbose == true)
            std::cout << "dinterrd_connect: client connected [" << ntohs(dsock->address.sin_port) << "]" << std::endl;

        /* while we haven't hit a terminal state
         * process socket io (sml::X) represents
         * the terminal state in the sml::sm object
         */
        while (sm->is(X) == false) {
            if (sm->is("load_init"_s) == true) {
                /*
                 * send a LOAD_REQUEST with the full path
                 * to the server
                 */
                ddtp_payload_init(LOAD_REQUEST, &pl);
                ddtp_payload_fill_char_data(&pl, filename);
                ddtp_client_send_payload(dsock, &pl);

                /*
                 * transition to "load_pend"_s state
                 * by processing load_request event
                 * just sent to server
                 */
                sm->process_event(load_request{});

                /*
                 * do a readwait on socket for either LOAD_FAIL
                 * or LOAD_SUCCEED payload from server...actually
                 * if the payload is not of type LOAD_SUCCESS,
                 * shutdown the client connection and free_and_fail
                 */
                dinterr_readwait(dsock, response, bsize, &data_stream);

                /*
                 * deserialize data received on socket to a
                 * ddtp_payload_t object
                 */
                DinterrSerdesNetwork* sd = ddtp_serdes_create(
                                             data_stream.c_str());
                ddtp_payload_t* srvpl = (ddtp_payload_t*) sd->get_data();

                if (srvpl->type == LOAD_SUCCEED) {
                    sm->process_event(load_success{});
                }
                else {
                    sm->process_event(load_fail{});
                    error_code = ddtp_payload_extract_short_data(srvpl);
                    std::cerr << "dinterrd_connect: LOAD_REQUEST Error: " << \
                                 ddtpError[error_code] << std::endl;
                    shutdown(dsock->cli_sockfd, SHUT_RDWR);
                }
                ddtp_serdes_destroy(sd);

                /*
                 * load_fail event will transition to terminal
                 * state, so free_and_fail
                 */
                if (sm->is(X) == true)
                    goto free_and_fail;
            }

            poll_sockfd[0].fd = dsock->conn_sockfd;
            poll_sockfd[0].events = POLLRDNORM;

            /*
             * making it this far implies we
             * poll on socket for dinterr data
             * stream from server
             *
             * IMPROVEMENT: verifying dinterr data
             *              via crc32 values already
             *              in data payload
             */
            poll(poll_sockfd, 1, 0);
            do {
                if(dinterr_sock_read(dsock, response, bsize) == SOCKIO_FAIL) {
                    goto free_and_fail;
                }
                
                /* DEBUG
                int i = 0;
                for (i; i <= sizeof(pl); i++) {
                    printf("0x%02X ", response[i]);
                }
                printf("\n----------\n");
                */
                ddtp_client_process_response(response, csv);
                memset(response, '\0', MAX_PAYLOAD_SIZE);
                poll(poll_sockfd, 1, 0);
            } while (poll_sockfd[0].revents & POLLRDNORM);

            if (iter_count >= iterations) {
                ddtp_payload_init(UNLOAD_REQUEST, &pl);
                ddtp_client_send_payload(dsock, &pl);
            }

            iter_count += 1;
        }
        return(SOCKIO_SUCCESS);
    }

free_and_fail:
    if (response)
        free(response);
    return(SOCKIO_FAIL);
}

/*
 * Analog: _ddtp_server_send_payload()
 */
int ddtp_client_send_payload(dinterr_sock_t* dsock, ddtp_payload_t* pl) {
    DinterrSerdesNetwork* cli_pl = new DinterrSerdesNetwork(pl);
    char* pl_data = (char*) cli_pl->get_data();
    int retval = dinterr_sock_write(dsock, (const char*) pl_data);
    delete cli_pl;
    return(retval);
}

void ddtp_client_process_response(const char* response, std::ofstream* csv) {
    DinterrSerdesData* cli_dat = new DinterrSerdesData(response);
    dinterr_data_t* d_data = (dinterr_data_t*) cli_dat->get_data();
    dump_dinterr_data_toCsv(csv, d_data);
    delete cli_dat;
}
