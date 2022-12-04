#ifndef TRANSMISSION_H
#define TRANSMISSION_H

struct TTransmissionConfig {
    long int nextevent;
    long int ourip_nextevent;
    long int webserver_nextevent;
    long int gps_nextevent;
    long int ping_rov_nextevent;
    unsigned short int modem_ready;
    unsigned short int modem_errors;
    unsigned short int modem_linked;
    unsigned short int rs485_ready;
    unsigned short int rs485_errors;
    unsigned long int http_link_wait;
    char ipaddr[17];
};
typedef struct TTransmissionConfig TransmissionConfig;

#endif
