#include "communication.h"

CommunicationConfig communication_config;

const char *CCOMMUNICATION="COMMUNICATION";

void communication_setup() {
    communication_config.alioli_protocol_buf = NULL;
    communication_config.alioli_protocol_buf_size = 0;
    communication_config.alioli_protocol_buf_allocated = 0;
}

void communication_reset() {
    communication_config.alioli_protocol_buf_size = 0;
}

unsigned short int remote_msg(char **buf, unsigned int *buf_size, unsigned int *buf_allocated, char **answer, unsigned int *answer_size, unsigned int *answer_allocated) {
    unsigned short int gotmsg=0;
    unsigned int buf_idx=0;

    // Put all together
    if (*buf_size) {
        strcat_realloc(&communication_config.alioli_protocol_buf, &communication_config.alioli_protocol_buf_size, &communication_config.alioli_protocol_buf_allocated, *buf, *buf_size, __FILE__, __LINE__);
    }
    // print_ashex(communication_config.alioli_protocol_buf, communication_config.alioli_protocol_buf_size, stderr);

    // While data in the buffer and not alioli_protocol message detected, keep reading
    yield();
    while (buf_idx<communication_config.alioli_protocol_buf_size) {

        // Parse alioli_protocol mg
        if (protocol_parse_char((byte) communication_config.alioli_protocol_buf[buf_idx], &(communication_config.alioli_protocol_msg), &(communication_config.alioli_protocol_status))) {

            // Say we got a alioli_protocol message
            gotmsg=1;

            switch(communication_config.alioli_protocol_msg.kind) {
                case ALIOLI_PROTOCOL_KIND_HEARTBEAT:
                    {
                        HeartBeat heartbeat;
                        char *msg=NULL;

                        // Unpack
                        if (protocol_unpack_heartbeat(&communication_config.alioli_protocol_msg, &heartbeat)) {
                            print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "HEARTBEAT: %ld delay", heartbeat.answered-heartbeat.requested);

                            // Set our timestamp
                            heartbeat.answered = get_current_time_with_ms();

                            // Attach answer
                            msg = (char*) protocol_pack_heartbeat(&heartbeat);
                            if (msg) {
                                strcat_realloc(answer, answer_size, answer_allocated, msg, ALIOLI_PROTOCOL_SIZE_HEARTBEAT, __FILE__, __LINE__);
                                free(msg);
                            }

                            // Check if environment was updated
                            if (rov.environment_updated) {

                                // Attach our environment
                                msg = (char*) protocol_pack_environment(&(rov.environment));
                                if (msg) {
                                    strcat_realloc(answer, answer_size, answer_allocated, msg, ALIOLI_PROTOCOL_SIZE_ENVIRONMENT, __FILE__, __LINE__);
                                    free(msg);
                                }

                            }

                        } else {
                            print_debug(CCOMMUNICATION, stderr, CRED, 0, "HEARTBEAT: wrong package");
                        }
                    }
                    break;

                case ALIOLI_PROTOCOL_KIND_USERREQUEST:
                    {

                        // Unpack
                        if (protocol_unpack_userrequest(&communication_config.alioli_protocol_msg, &rov.userrequest)) {

                            // Got a valid userrequest
                            print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "UserRequest: (%d, %d, %d, %d)", rov.userrequest.x, rov.userrequest.y, rov.userrequest.z, rov.userrequest.r);

                        } else {
                            print_debug(CCOMMUNICATION, stderr, CRED, 0, "UserRequest: wrong package");
                        }
                    }
                    break;

                default:
                    //Do nothing
                    print_debug(CCOMMUNICATION, stdout, CYELLOW, 0, "ALIOLI PROTOCOL Unknown message with KIND %d", communication_config.alioli_protocol_msg.kind);
            }
        }

        // Visit next character
        buf_idx++;

    }
    yield();

    // Move back what is left to the beginning of the buffer
    if (buf_idx<communication_config.alioli_protocol_buf_size) {
        // Left some bytes for reading in the future
        memmove(communication_config.alioli_protocol_buf, communication_config.alioli_protocol_buf+buf_idx,communication_config.alioli_protocol_buf_size-buf_idx);
    } else {
        // Everything readen
        communication_config.alioli_protocol_buf_size = 0;
    }

    // If something in the buffer, warn about it!
    if (communication_config.alioli_protocol_buf_size) {
        print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "ALIOLI PROTOCOL Buffer: %d", communication_config.alioli_protocol_buf_size);
    }

    // We didn't get any alioli_protocol message
    if (!gotmsg) {
        print_debug(CCOMMUNICATION, stderr, CRED, 0, "ERROR: didn't get an ALIOLI PROTOCOL message");
    }

    // Return result
    return gotmsg;

}
