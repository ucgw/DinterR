#include "server.h"

int dinterrd_processor(char* cli_addr, uint16_t src_port) {
    sml::sm<ddtp_server> sm;
    using namespace sml;
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
