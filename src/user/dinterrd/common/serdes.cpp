#include <stdlib.h>
#include <string.h>

#include "dataproto.h"

void DinterrSerdesData::_copy_push_along_ptrs(size_t s, const char *data, dinterr_data_t *ddata) {
    if (data && ddata) {
        memcpy(ddata, data, s);
        ddata += s;
        data += s;
    }
}

DinterrSerdesData::DinterrSerdesData(const dinterr_data_t *data) {
    /* serialize dinterr_data_t -> byte array */
    this->_serdes = (char*)malloc(sizeof(dinterr_data_t));
    if (this->_serdes)
        memcpy(this->_serdes, (char*)data, sizeof(dinterr_data_t));
}

DinterrSerdesData::DinterrSerdesData(const char *data) {
    /* deserialize byte array -> dinterr_data_t */
    size_t field_size, dinterr_data_size, input_data_size;

    dinterr_data_size = sizeof(dinterr_data_t);
    input_data_size = sizeof(*data);

    if (input_data_size == dinterr_data_size) {
        this->_serdes = (dinterr_data_t*)malloc(dinterr_data_size);
        if (this->_serdes) {
            // _crc
            field_size = sizeof(uLong);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _attrs
            field_size = sizeof(unsigned char);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _pos
            field_size = sizeof(dinterr_pos_t);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _count
            field_size = sizeof(dinterr_count_t);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _pid
            field_size = sizeof(dinterr_pid_t);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _ra_page_count
            field_size = sizeof(unsigned int);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _ra_cache_misses
            field_size = sizeof(unsigned int);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _ra_last_cache_pos
            field_size = sizeof(dinterr_pos_t);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);

            // _timestamp
            field_size = sizeof(dinterr_ts_t);
            _copy_push_along_ptrs(field_size, data,
                                  (dinterr_data_t*)this->_serdes);
        }
    }
}

DinterrSerdesData::~DinterrSerdesData() {
    if (this->_serdes)
        free(this->_serdes);
}

void* DinterrSerdesData::get_data(void) {
    return this->_serdes;
}
