#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <iostream>

#include "serdes.h"

/*
 * the major assumption is that both systems communicating
 * together are of the same endianness. for this project's
 * purpose, we will be using x86 systems, so the byte array
 * will be in litte-endian format.
 */

DinterrSerdesData::DinterrSerdesData(const dinterr_data_t *data) {
    /* serialize dinterr_data_t -> byte array */
    this->_serdes = (char*)malloc(sizeof(dinterr_data_t));
    if (this->_serdes)
        memcpy(this->_serdes, (char*)data, sizeof(dinterr_data_t));
}

DinterrSerdesNetwork::DinterrSerdesNetwork(const ddtp_msg_t *data) {
    /* serialize ddtp_msg_t -> byte array */
    this->_serdes = (char*)malloc(sizeof(*data));
    if (this->_serdes)
        memcpy(this->_serdes, (char*)data, sizeof(*data));
}

DinterrSerdesData::DinterrSerdesData(const char *data) {
    /* deserialize byte array -> dinterr_data_t */
    size_t field_size;

    this->_serdes = (dinterr_data_t*)malloc(sizeof(dinterr_data_t));
    if (this->_serdes)
        memcpy(this->_serdes, (dinterr_data_t*)data, sizeof(dinterr_data_t));
}

DinterrSerdesNetwork::DinterrSerdesNetwork(const char *data) {
    /* deserialize byte array -> ddtp_msg_t */
    size_t field_size;

    /* we malloc sizeof() dereferenced input data because
     * a ddtp_msg_t has a void* member which may in fact
     * be variably sized in contrast to a dinterr_data_t
     */
    this->_serdes = (ddtp_msg_t*)malloc(sizeof(*data));
    if (this->_serdes)
        memcpy(this->_serdes, (ddtp_msg_t*)data, sizeof(*data));
}

DinterrSerdes::~DinterrSerdes() {
    if (this->_serdes)
        free(this->_serdes);
}

void* DinterrSerdes::get_data(void) {
    return this->_serdes;
}
