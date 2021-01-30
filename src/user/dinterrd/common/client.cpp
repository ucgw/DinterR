#include "client.h"

int dinterrd_run_client(dinterr_sock_t* dsock, uint16_t port, const char* ipaddr, const char* filename, const char* csvfile, bool verbose) {
    using namespace sml;
    sml::sm<ddtp_client> sm;
    std::ofstream csv;

    if (open_for_append(csvfile, &csv) == false)
        return(SOCKIO_FAIL);

    dinterr_sock_init(dsock, DINTERR_CLIENT, ipaddr);
    int retval = dinterr_sock_create(dsock, port, verbose);

    if (retval == SOCKIO_SUCCESS)
        return(dinterrd_connect(dsock, &sm, filename, &csv));
    csv.close();
    return(retval);
}

int dinterrd_connect(dinterr_sock_t* dsock, sml::sm<ddtp_client>* sm, const char* filename, std::ofstream* csv) {
    using namespace sml;
    struct pollfd poll_sockfd[1];
    int connsock = NOSOCKFD;
    int connect_retval = 0;
    char* response = NULL;
    ddtp_payload_t pl;

    if (dsock->type == DINTERR_CLIENT) {
        connect_retval = connect(dsock->cli_sockfd,
                           (struct sockaddr*)&dsock->address,
                           (socklen_t)dsock->addrlen);

        if (connect_retval == -1) {
            perror("dinterrd_connect: connect()");
            goto free_and_fail;
        }

        response = (char*)malloc(sizeof(char) * MAX_PAYLOAD_SIZE);

        if (response == NULL) {
            shutdown(dsock->cli_sockfd, SHUT_RDWR);
            std::cerr << "dinterrd_connect: response buffer malloc() failed" << std::endl;
            goto free_and_fail;
        }

        if (dsock->verbose == true)
            std::cout << "dinterrd_connect: client connected [" << ntohs(dsock->address.sin_port) << "]" << std::endl;

        while (sm->is(X) == false) {
            if (sm->is("load_init"_s) == true) {
                ddtp_payload_init(LOAD_REQUEST, &pl);
                ddtp_payload_fill_char_data(&pl, filename);
                ddtp_client_send_payload(dsock, &pl);
                sm->process_event(load_success{});
            }

            poll_sockfd[0].fd = dsock->conn_sockfd;
            poll_sockfd[0].events = POLLRDNORM;

            poll(poll_sockfd, 1, 0);
            do {
                dinterr_sock_read(dsock, response, MAX_PAYLOAD_SIZE);
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
        }
        return(SOCKIO_SUCCESS);
    }

free_and_fail:
    if (response)
        free(response);
    return(SOCKIO_FAIL);
}

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
