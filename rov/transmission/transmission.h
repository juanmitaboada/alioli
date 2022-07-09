#ifndef TRANSMISSION_H
#define TRANSMISSION_H

struct TTransmissionConfig {
    long int nextevent;
    char *mavlink_buf;
    unsigned int mavlink_buf_size;
    unsigned int mavlink_buf_allocated;
};
typedef struct TTransmissionConfig TransmissionConfig;

#endif
