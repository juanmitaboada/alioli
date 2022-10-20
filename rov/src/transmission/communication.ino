#include "communication.h"

CommunicationConfig communication_config;

const char *CCOMMUNICATION="COMMUNICATION";

void communication_setup() {
    communication_config.alioli_protocol_buf = NULL;
    communication_config.alioli_protocol_buf_size = 0;
    communication_config.alioli_protocol_buf_allocated = 0;
    protocol_setup_package(&communication_config.alioli_protocol_msg);
    protocol_setup_status(&communication_config.alioli_protocol_status);
}

void communication_reset() {
    communication_config.alioli_protocol_buf_size = 0;
    protocol_setup_package(&communication_config.alioli_protocol_msg);
    protocol_setup_status(&communication_config.alioli_protocol_status);
}

unsigned short int remote_msg(char **buf, unsigned int *buf_size, unsigned int *buf_allocated, char **answer, unsigned int *answer_size, unsigned int *answer_allocated, long int now) {
    unsigned short int gotmsg=0;
    unsigned int buf_idx=0;

    // Put all together
    if (*buf_size) {
        strcat_realloc(&communication_config.alioli_protocol_buf, &communication_config.alioli_protocol_buf_size, &communication_config.alioli_protocol_buf_allocated, *buf, *buf_size, __FILE__, __LINE__);
    }

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

                        // Unpack
                        if (protocol_unpack_heartbeat(&communication_config.alioli_protocol_msg, &heartbeat)) {
#if DEBUG_COMMUNICATION
#if OPTIMIZE
                            Serial.print(F("HEARTBEAT: Delay="));
                            Serial.println(heartbeat.answered-heartbeat.requested);
#else
                            print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "HEARTBEAT: %ld delay - Available RAM=%d", heartbeat.answered-heartbeat.requested, availableRAM());
#endif
#endif

                            // Set our timestamp
                            heartbeat.answered = (int32_t) get_current_time();

                            // Attach answer
                            protocol_pack_heartbeat(&heartbeat, communication_config.alioli_protocol_msg.counter, answer, answer_size, answer_allocated);

                            // Check if environment was updated
                            if (rov.environment_newdata) {

                                // Attach our environment
                                protocol_pack_environment(&(rov.environment), protocol_counter, answer, answer_size, answer_allocated);
                                if (protocol_counter==255) {
                                    protocol_counter = 1;
                                } else {
                                    protocol_counter++;
                                }

                            }

#if DEBUG_COMMUNICATION
                        } else {
#if OPTIMIZE
                            Serial.println(F("HEARTBEAT: wrong package"));
#else
                            print_debug(CCOMMUNICATION, stderr, CRED, 0, "HEARTBEAT: wrong package");
#endif
#endif
                        }
                    }
                    break;

                case ALIOLI_PROTOCOL_KIND_USERREQUEST:
                    {

                        // Unpack
                        if (protocol_unpack_userrequest(&communication_config.alioli_protocol_msg, &rov.userrequest)) {

                            Serial.print(F("UserRequest: X="));
                            Serial.print(rov.userrequest.x);
                            Serial.print(F(", Y="));
                            Serial.print(rov.userrequest.y);
                            Serial.print(F(", Z="));
                            Serial.print(rov.userrequest.z);
                            Serial.print(F(", R="));
                            Serial.print(rov.userrequest.r);
                            Serial.println();

                            // Got a valid userrequest
#if DEBUG_COMMUNICATION
#if OPTIMIZE
                            Serial.print(F("UserRequest: X="));
                            Serial.print(rov.userrequest.x);
                            Serial.print(F(", Y="));
                            Serial.print(rov.userrequest.y);
                            Serial.print(F(", Z="));
                            Serial.print(rov.userrequest.z);
                            Serial.print(F(", R="));
                            Serial.print(rov.userrequest.r);
                            Serial.println();
#else
                            print_debug(CCOMMUNICATION, stdout, CWHITE, 0, "UserRequest: (%d, %d, %d, %d)", rov.userrequest.x, rov.userrequest.y, rov.userrequest.z, rov.userrequest.r);
#endif
#endif
#if DEBUG_COMMUNICATION
                        } else {
#if OPTIMIZE
                            Serial.println(F("UserRequest: wrong package"));
#else
                            print_debug(CCOMMUNICATION, stderr, CRED, 0, "UserRequest: wrong package");
#endif
#endif
                        }
                    }
                    break;

#if DEBUG_COMMUNICATION
                default:
#if OPTIMIZE
                    Serial.print(F("ALIOLI PROTOCOL Unknown message kind: "));
                    Serial.println(communication_config.alioli_protocol_msg.kind);
#else
                    //Do nothing
                    print_debug(CCOMMUNICATION, stdout, CYELLOW, 0, "ALIOLI PROTOCOL Unknown message with KIND %d", communication_config.alioli_protocol_msg.kind);
#endif
#endif
            }

            // Make sure that reading package doesn't cost us memory
            protocol_free_package(&communication_config.alioli_protocol_msg);

        }

        // Visit next character
        buf_idx++;

    }
    yield();

    // Move back what is left to the beginning of the buffer
    if (buf_idx<communication_config.alioli_protocol_buf_size) {
        // Left some bytes for reading in the future
        memmove(communication_config.alioli_protocol_buf, communication_config.alioli_protocol_buf+buf_idx, communication_config.alioli_protocol_buf_size-buf_idx);
        // Set new size
        communication_config.alioli_protocol_buf_size -= buf_idx;
    } else {
        // Everything readen
        communication_config.alioli_protocol_buf_size = 0;
    }

    // If something in the buffer, warn about it!
#if DEBUG_COMMUNICATION
    if (communication_config.alioli_protocol_buf_size) {
#if OPTIMIZE
        Serial.print(F("ALIOLI PROTOCOL BUFFER: "));
        Serial.println(communication_config.alioli_protocol_buf_size);
#else
        print_debug(CCOMMUNICATION, stdout, CBLUE, 0, "ALIOLI PROTOCOL Buffer: %d", communication_config.alioli_protocol_buf_size);
#endif
    }
#endif

    // We didn't get any alioli_protocol message
#if DEBUG_COMMUNICATION
    if (!gotmsg) {
#if OPTIMIZE
        Serial.println(F("ERROR: didn't get an ALIOLI PROTOCOL message"));
#else
        print_debug(CCOMMUNICATION, stderr, CRED, 0, "ERROR: didn't get an ALIOLI PROTOCOL message");
#endif
    }
#endif

    // Return result
    return gotmsg;

}
