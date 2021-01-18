#include <iostream>
#include <cstring>

#include "netproto.h"
#include "sockio.h"
#include "serdes.h"
#include "payload.h"

int main(int argc, char** argv) {
    std::string fpath = "/home/dinter/csce499/DinterR/src/user/dinterrd/bigfile";
    uLong crc = 431424720;

    ddtp_payload_t pl;
    ddtp_payload_t* pl2;

    ddtp_payload_init(LOAD_REQUEST, &pl);
    //ddtp_payload_fill_char_data(&pl, fpath.c_str());
    ddtp_payload_fill_ulong_data(&pl, crc);

    DinterrSerdesNetwork* cli_pl = new DinterrSerdesNetwork(&pl);
    char* data = (char*) cli_pl->get_data();
    int i = 0;
    for (i; i <= sizeof(pl); i++) {
        printf("%02X ", data[i]);
    }
    delete cli_pl;

    DinterrSerdesNetwork* cli_pl2 = new DinterrSerdesNetwork(data);
    pl2 = (ddtp_payload_t*) cli_pl2->get_data();
    uLong pl2_ulong = ddtp_payload_extract_ulong_data(pl2);

    std::cout << "Type: " << pl2->type << " Data: " << pl2_ulong << std::endl;

    delete cli_pl2;

}
