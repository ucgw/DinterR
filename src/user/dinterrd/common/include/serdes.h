#ifndef _SERDES_H_
#define _SERDES_H_

#include "dataproto.h"
#include "netproto.h"

/* different constructors based on input data type
 * determines how class object is used.
 *
 * (i.e. whether object serializes or deserializes)
 * serialize => input: dinterr_data_t*
 * serialize => input: ddtp_msg_t*
 * deserialize => input: char*
 */

class DinterrSerdes {
    protected:
        void *_serdes;
    public:
        ~DinterrSerdes();
        void* get_data(void);
};

class DinterrSerdesData:public DinterrSerdes {
    public:
        DinterrSerdesData(const dinterr_data_t*); // serialize mode
        DinterrSerdesData(const char*); // deserialize mode
        
};

class DinterrSerdesNetwork:public DinterrSerdes {
    public:
        DinterrSerdesNetwork(const ddtp_msg_t*); // serialize mode
        DinterrSerdesNetwork(const char*); // deserialize mode
};

#endif // _SERDES_H_
