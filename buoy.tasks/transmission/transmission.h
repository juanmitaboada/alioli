#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "freertos/semphr.h"

typedef struct TBusLine {
    char *buf;
    unsigned int size;
    unsigned int allocated;
    SemaphoreHandle_t lock;
} BusLine;

typedef struct TBus {
    BusLine in;
    BusLine out;
    unsigned short int port;
} Bus;

struct TTransmissionConfig {
    long int nextevent;
    long int gps_nextevent;
    unsigned short int modem_step;
    unsigned short int modem_ready;
    unsigned short int modem_errors;
    unsigned short int modem_linked;
    unsigned short int rs485_ready;
    unsigned short int rs485_errors;
    Bus modem;
    Bus rs485; 
    char *mavlink_buf;
    unsigned int mavlink_buf_size;
    unsigned int mavlink_buf_allocated;
    char ipaddr[17];
};
typedef struct TTransmissionConfig TransmissionConfig;

#endif
