#include "util.h"

bool open_for_append(const char* filename, std::ofstream* outfile) {
    bool retval = false;
    outfile->open(filename, std::ios::app);

    if (outfile->is_open())
        retval = true;
    else
        std::cerr << "Error: output file stream reference invalid for " << filename << std::endl;

    return(retval);
}

void dump_dinterr_data_toCsv(std::ofstream* outfile, dinterr_data_t* data) {
    if (outfile->is_open()) {
        *outfile << data->_crc << \
             "," << (int) data->_attrs << \
             "," << (dinterr_pos_t) data->_pos << \
             "," << (dinterr_count_t) data->_count << \
             "," << (dinterr_pid_t) data->_pid << \
             "," << data->_ra_page_count << \
             "," << data->_ra_cache_misses << \
             "," << (dinterr_pos_t) data->_ra_last_cache_pos << \
             "," << data->_timestamp.tv_sec << \
             "." << data->_timestamp.tv_usec << \
             std::endl;
    }
}
