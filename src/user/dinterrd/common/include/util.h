#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <fstream>

#include "dataproto.h"

bool open_for_append(const char*, std::ofstream*);
void dump_dinterr_data_toCsv(std::ofstream*, dinterr_data_t*);

#endif // _UTIL_H_
