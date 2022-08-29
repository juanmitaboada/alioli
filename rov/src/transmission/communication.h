#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "lib/constants.h"

typedef struct TCommunicationConfig {
    char *mavlink_buf;
    unsigned int mavlink_buf_size;
    unsigned int mavlink_buf_allocated;
} CommunicationConfig;


void communication_setup();
void communication_reset();
unsigned short int remote_msg(char **buf, unsigned int *buf_size, unsigned int *buf_allocated, char **answer, unsigned int *answer_size, unsigned int *answer_allocated);
unsigned short int rov_msg(char *buf, unsigned int buf_size, char **answer, unsigned int *answer_size, unsigned int *answer_allocated);

#endif
