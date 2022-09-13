#ifndef BUOY_H
#define BUOY_H

#include "common/protocol.h"

struct TBuoy {
    GPS gps;
    unsigned short int gps_newdata;
    Acelerometer acelerometer;
    unsigned short int acelerometer_newdata;
    Environment environment;
    UserRequest userrequest;
    unsigned short int environment_newdata;
};
typedef struct TBuoy Buoy;

#endif
