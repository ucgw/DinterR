#include "server.h"

int dinterrd_processor(dinterr_sock_t* dsock, char* cli_addr, uint16_t src_port) {
    sml::sm<ddtp_server> sm;
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
        while (sm.is(sml::X) == false) {
           /* wait for something to come over the wire
            * and consume the stream ultimately as a
            * char array via the data_stream object.
            * this in turn will be deserialized into
            * a ddtp_payload_t object
            */
            dinterr_readwait(dsock, buffer, &data_stream);
            DinterrSerdesNetwork* sd = ddtp_serdes_create(data_stream.c_str());
            ddtp_payload_t* pl = (ddtp_payload_t*) sd->get_data();
            printf("payload type: %d (0x%02X)\n", pl->type, pl->type);
            ddtp_serdes_destroy(sd);
            break;
        }

        free(buffer);
    }

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
    return(0);
}
