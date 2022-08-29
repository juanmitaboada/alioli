#include "communication.h"

CommunicationConfig communication_config;

const char *CCOMMUNICATION="COMMUNICATION";

void communication_setup() {
    communication_config.mavlink_buf = NULL;
    communication_config.mavlink_buf_size = 0;
    communication_config.mavlink_buf_allocated = 0;
}

void communication_reset() {
    communication_config.mavlink_buf_size = 0;
}

unsigned short int remote_msg(char **buf, unsigned int *buf_size, unsigned int *buf_allocated, char **answer, unsigned int *answer_size, unsigned int *answer_allocated) {
    unsigned short int gotmsg=0, incomingByte=0, bypass=0;
    unsigned int buf_idx=0;

    // Return result
    return gotmsg;
}
