#include "server.h"

#include "cxxopts.hpp"

/* default port to use based on the listing of iana
 * registered ports is 8992
 * (unassigned as of 2021-01-15)
 *
 * Reference:
 *   https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.txt
 */

#define DEFAULT_ALL_IPS "ALL"

int main(int argc, char** argv) {
    std::string ddtp_ipaddr;
    uint16_t ddtp_port;
    bool verbose;
    dinterr_sock_t dsock;
    int iterations = 0;
    int sockretval = SOCKIO_FAIL;

    cxxopts::Options options("ddtp-serve", "A DinterR data tranport protocol server");
    options
      .add_options()
      ("i,ipaddr", "ip address to bind socket to",
       cxxopts::value<std::string>()->default_value(DEFAULT_ALL_IPS))
      ("p,port", "remote ddtp server port to connect to",
       cxxopts::value<uint16_t>()->default_value("8992"))
      ("v,verbose", "verbose flag",
       cxxopts::value<bool>()->default_value("false"))
      ("h,help", "Print usage")
    ;

    try {
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cerr << options.help() << std::endl;
            exit(0);
        }

        verbose = result["verbose"].as<bool>();
        ddtp_ipaddr = result["ipaddr"].as<std::string>();
        ddtp_port   = result["port"].as<uint16_t>();
    }
    catch (const cxxopts::OptionException& argerr) {
        std::cerr << "Error parsing: " << argerr.what() << std::endl;
        std::cerr << options.help() << std::endl;
        exit(1);
    }

    if (ddtp_ipaddr.compare(DEFAULT_ALL_IPS) == 0)
        dinterrd_run_server(&dsock, ddtp_port);
    else
        dinterrd_run_server(&dsock, ddtp_port, ddtp_ipaddr.c_str());
}
