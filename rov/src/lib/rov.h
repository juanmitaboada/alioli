#ifndef ROV_H
#define ROV_H

#include "common/protocol.h"

struct TRov {
    UserRequest userrequest;
    Environment environment;
    unsigned short int environment_newdata;
};
typedef struct TRov Rov;

#endif
