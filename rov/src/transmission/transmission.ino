#include "lib/shared.h"
#include "lib/common/alioli.h"
#include "lib/common/serial.h"

#include "transmission.h"
#include "communication.h"

TransmissionConfig transmission_config;

unsigned short int rs485_getmsg(char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait, unsigned int wait_transfer) {
    unsigned short int error=0;

    // Initialize
    (*buf_size) = 0;


    error = !serial_recv(RS485_SERIAL, buf, buf_size, buf_allocated, wait, wait_transfer, 0);

    return !error;
}

unsigned short int rs485_sendmsg(char *buf, size_t buf_size) {
    return serial_send(RS485_SERIAL, buf, buf_size);
}

void transmission_setup(long int now) {
#if DEBUG_TRANSMISSION
    print_debug("TRANSMISSION-SETUP", stdout, CPURPLE, COLOR_NOTAIL, "INI -> ");
#endif

    // Setup serial ports
    serial_setup();
    communication_setup();

    // Set local config
    transmission_config.nextevent = 0;

#if DEBUG_TRANSMISSION
    // Show we are done
    print_debug("TRANSMISSION-SETUP", stdout, CGREEN, COLOR_NOHEAD, "DONE");
#endif
}

void transmission_loop(long int now) {
    char *buf=NULL, *answer=NULL;
    unsigned int buf_size=0, buf_allocated=0, answer_size=0, answer_allocated=0;

    // Check transmission lookup
    if (transmission_config.nextevent<now) {

        // Set next event
        transmission_config.nextevent = now+TRANSMISSION_MS;


        // Get answer
        buf_size = 0;
        if (rs485_getmsg(&buf, &buf_size, &buf_allocated, TIMEOUT_RS485_MS, TRANSFER_RS485_WAIT_MS)) {

            // Check if there is a message to process
            if (buf_size) {

                    // Message detected
#if DEBUG_TRANSMISSION_MSG
                    print_debug("TRl", stdout, CPURPLE, 0, "MSG:");
                    print_asbin(buf, buf_size, stderr);
                    print_ashex(buf, buf_size, stderr);
#endif

                    // HELLO msg
                    if (!bistrcmp(buf, min(buf_size, (unsigned int) 4), "PING", 4)) {
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "PING -> PONG");
#endif
                        answer_size = 0;
                        strcat_realloc(&answer, &answer_size, &answer_allocated, "PONG\n", 5, __FILE__, __LINE__);
                        communication_reset();

                    // Remote msg
                    } else {
                        // Process message locally if linked to external system
#if DEBUG_TRANSMISSION_MSG
                        print_debug("TRl", stdout, CCYAN, 0, "REMOTE MSG");
#endif
                        answer_size = 0;
                        remote_msg(&buf, &buf_size, &buf_allocated, &answer, &answer_size, &answer_allocated);
                    }

                // Send message to RS485
                if (answer_size) {
                    rs485_sendmsg(answer, answer_size);
                    answer_size = 0;
                }
            }
        }

        // Free memory
        if (buf) {
            free(buf);
            buf = NULL;
            buf_size = 0;
        }
        if (answer) {
            free(answer);
            answer = NULL;
            answer_size = 0;
        }


    }

}
