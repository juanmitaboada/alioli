#ifndef TRANSMISSION_H
#define TRANSMISSION_H

struct TTransmissionConfig {
    long int nextevent;
    long int gps_nextevent;
    unsigned short int modem_ready;
    unsigned short int modem_errors;
    unsigned short int modem_linked;
    unsigned short int rs485_ready;
    unsigned short int rs485_errors;
    char ipaddr[17];
};
typedef struct TTransmissionConfig TransmissionConfig;

#endif
