#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <iostream>

#include "dataproto.h"

DinterrSerdesData::DinterrSerdesData(const dinterr_data_t *data) {
    /* serialize dinterr_data_t -> byte array */
    this->_serdes = (char*)malloc(sizeof(dinterr_data_t));
    if (this->_serdes)
        memcpy(this->_serdes, (char*)data, sizeof(dinterr_data_t));
}

DinterrSerdesData::DinterrSerdesData(const char *data) {
    /* deserialize byte array -> dinterr_data_t */
    size_t field_size;

    this->_serdes = (dinterr_data_t*)malloc(sizeof(dinterr_data_t));
    if (this->_serdes)
        memcpy(this->_serdes, (dinterr_data_t*)data, sizeof(dinterr_data_t));
}

DinterrSerdesData::~DinterrSerdesData() {
    if (this->_serdes)
        free(this->_serdes);
}

void* DinterrSerdesData::get_data(void) {
    return this->_serdes;
}
