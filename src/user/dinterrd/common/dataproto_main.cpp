#include <stdio.h>
#include <iostream>

#include "dataproto.cpp"

int main(int argc, char **argv) {
    DinterrSerdesData *serdes;
    DinterrSerdesData *unserdes;
    
    dinterr_count_t count = 11;
    dinterr_pos_t pos = 25;
    dinterr_pid_t pid = 1246;
    dinterr_ts_t timestamp;
    dinterr_pos_t ra_last_cache_pos = 1900;
    unsigned int ra_page_count = 1;
    unsigned int ra_cache_misses = 1;
    unsigned char attrs = 1;

    int i = 0;
    char *serial_data;
    dinterr_data_t *unserial_data;

    timestamp.tv_usec = 788871;
    timestamp.tv_sec = 19;

    set_attr(&attrs, DINTERR_ATTR_READAHEAD);

    auto test = dinterrRecord()
                .add_attrs(attrs)
                .add_pos(pos)
                .add_count(count)
                .add_pid(pid)
                .add_ra_page_count(ra_page_count)
                .add_ra_cache_misses(ra_cache_misses)
                .add_ra_last_cache_pos(ra_last_cache_pos)
                .add_timestamp(&timestamp)
                .calc_crc();

    std::cout << test.get_crc() << std::endl;
    serdes = test.get_serdes();
    serial_data = (char*)serdes->get_data();

    for (i; i < sizeof(dinterr_data_t); i++) {
        printf("%02X ", serial_data[i]);
    }
    std::cout << std::endl;

    unserdes = new DinterrSerdesData(serial_data);
    unserial_data = (dinterr_data_t*)unserdes->get_data();
    std::cout << unserial_data->_pid << std::endl;

    /* downside is that these serdes objects must be deleted
     * because they are allocated on the heap.
     */
    delete unserdes;
    delete serdes;
}
