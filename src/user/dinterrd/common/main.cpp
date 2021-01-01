#include <iostream>

#include "proto.cpp"

int main(int argc, char **argv) {
    dinterr_count_t count = 11;
    dinterr_pos_t pos = 25;
    dinterr_pid_t pid = 1246;
    dinterr_ts_t timestamp;
    dinterr_pos_t ra_last_cache_pos = 1900;
    unsigned int ra_page_count = 1;
    unsigned int ra_cache_misses = 1;
    unsigned char attrs = 1;

    timestamp.tv_usec = 788871;
    timestamp.tv_sec = 19;

    //set_attr(&attrs, DINTERR_ATTR_READAHEAD);

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
}
