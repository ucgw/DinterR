#include "client.h"

#include "cxxopts.hpp"

/* default port to use based on the listing of iana
 * registered ports is 8992
 * (unassigned as of 2021-01-15)
 *
 * Reference:
 *   https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.txt
 */

int main(int argc, char** argv) {
    std::string ddtp_server;
    std::string ddtp_rfile;
    std::string ddtp_csvfile;
    uint16_t ddtp_port;
    bool verbose;
    dinterr_sock_t dsock;
    int iterations = 0;
    int sockretval = SOCKIO_FAIL;

    cxxopts::Options options("ddtp-cli", "A DinterR data tranport protocol client");
    options
      .add_options()
      ("f,file", "remote file to watch via inotify for extra data",
       cxxopts::value<std::string>())
      ("c,csv", "csv file to write/append inotify extra data",
       cxxopts::value<std::string>())
      ("s,server", "remote ddtp server hostname/ip to connect to",
       cxxopts::value<std::string>())
      ("i,iterations", "number of poll interations on socket",
       cxxopts::value<int>()->default_value("10"))
      ("p,port", "remote ddtp server port to connect to",
       cxxopts::value<uint16_t>()->default_value("8992"))
      ("v,verbose", "verbose flag",
       cxxopts::value<bool>()->default_value("false"))
      ("h,help", "Print usage")
    ;

    try {
        auto result = options.parse(argc, argv);

        verbose = result["verbose"].as<bool>();
        iterations   = result["iterations"].as<int>();
        ddtp_server  = result["server"].as<std::string>();
        ddtp_rfile   = result["file"].as<std::string>();
        ddtp_csvfile = result["csv"].as<std::string>();
        ddtp_port    = result["port"].as<uint16_t>();
    }
    catch (const cxxopts::OptionException& argerr) {
        std::cerr << "Error parsing: " << argerr.what() << std::endl;
        std::cerr << options.help() << std::endl;
        exit(1);
    }

    return(dinterrd_run_client(&dsock, iterations,
              ddtp_port, ddtp_server.c_str(),
                  ddtp_rfile.c_str(), ddtp_csvfile.c_str(),
                      verbose));
}
