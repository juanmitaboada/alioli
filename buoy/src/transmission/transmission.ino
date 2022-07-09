// #include "lib/shared.h"
// #include "lib/config.h"
#include "lib/version.h"
#include "lib/common/alioli.h"
#include "lib/common/serial.h"

#include "transmission.h"
#include "communication.h"

TransmissionConfig transmission_config;

#ifdef MODEM_SERIAL
unsigned short int modem_cmd(const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    return serial_cmd(MODEM_SERIAL, "TR-M", cmd, expected, buf, buf_size, buf_allocated, wait);
}

unsigned short int modem_setup() {
    char *buf=NULL, *cmd=NULL, *pointer=NULL;
    size_t buf_size=0, buf_allocated=0;
    const char *msg=NULL;
    unsigned short int error=0, pin_ready=0, i=0, j=0, stat=0, retry=0;

    // Prepare cmd
    cmd = (char*) malloc(sizeof(char)*400);
    cmd[0]=0;

    // Reset modem status
    transmission_config.modem_ready = 0;
    transmission_config.modem_linked = 0;

    // Show information
#if DEBUG_MODEM_SETUP
    print_debug(NULL, stdout, CBLUE, COLOR_NOHEAD_NOTAIL, "MODEM=VERBOSE ");
#endif
#if MODEM_SIMULATED==1
    print_debug(NULL, stderr, CYELLOW, COLOR_NOHEAD_NOTAIL, "MODEM=SIMULATED ");
#endif


    // Start CMD Mode
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "+++");
#endif
            error = !modem_cmd("+++", NULL, &buf, &buf_size, &buf_allocated, 500);
    }

#if MODEM_SIMULATED==0
    // Get back from any process
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CTRL+Z");
#endif
        error = !modem_cmd("", NULL, &buf, &buf_size, &buf_allocated, 500);
    }
#endif

    // Reset modem
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "RESET");
#endif
        error = !modem_cmd("ATZ\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        delay(2000);
    }

    // Remove ECHO
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "ECHO OFF");
#endif
        error = !modem_cmd("ATE0\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000);
    }

    // Check AT
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CHECK AT");
#endif
        error = !modem_cmd("AT\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        if (!error) {
            if (!bistrstr(buf, buf_size, "OK", 2)) {
                // No answer to PIN status
                print_debug("TRsm", stderr, CRED, 0, "AT command has failed!");
                print_asbin(buf, buf_size);
                print_ashex(buf, buf_size);
                error = 1;
            }
        }
    }


#if MODEM_SIMULATED==0

    // RESET Module
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Reset MODULE (10 sec)");
#endif
        error = !modem_cmd("AT+CFUN=6\r\n", "\r\nOK", &buf, &buf_size, &buf_allocated, 10000);
    }

    // Startup Module
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Enable MODULE (15 sec)");
#endif
        error = !modem_cmd("AT+CFUN=1\r\n", NULL, &buf, &buf_size, &buf_allocated, 15000);
    }

    // Reset modem
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "RESET");
#endif
        error = !modem_cmd("ATZ\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        delay(2000);
    }

    // Remove ECHO
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "ECHO OFF");
#endif
        error = !modem_cmd("ATE0\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000);
    }

    // Check AT
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CHECK AT");
#endif
        error = !modem_cmd("AT\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        if (!error) {
            if (!bistrstr(buf, buf_size, "OK", 2)) {
                // No answer to PIN status
                print_debug("TRsm", stderr, CRED, 0, "AT command has failed!");
                print_asbin(buf, buf_size);
                print_ashex(buf, buf_size);
                error = 1;
            }
        }
    }

    // Check PIN status
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CHECK PIN STATUS");
#endif
        error = !modem_cmd("AT+CPIN?\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000);
        if (!error) {
            sprintf(cmd, "+CPIN: READY");
            if (bistrstr(buf, buf_size, cmd, strlen(cmd))) {
#if DEBUG_MODEM_SETUP
                print_debug("TRsm", stdout, CCYAN, 0, "PIN OK");
#endif
                pin_ready = 1;
            } else {
                // Check if waiting for PIN
                sprintf(cmd, "+CPIN: SIM PIN");
                if (!bistrstr(buf, buf_size, cmd, strlen(cmd))) {
                    // No answer to PIN status
                    print_debug("TRsm", stderr, CRED, 0, "NOT WAITING FOR PIN");
                    print_asbin(buf, buf_size);
                    print_ashex(buf, buf_size);
                    error = 1;
#if DEBUG_MODEM_SETUP
                } else {
                    print_debug("TRsm", stdout, CCYAN, 0, "WAITING FOR PIN");
#endif
                }
            }
        }
    }

    // Authenticate
    if (!error && !pin_ready) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "SEND PIN");
#endif
        sprintf(cmd, "AT+CPIN=%d\r\n", MODEM_PIN);
        error = !modem_cmd(cmd, NULL, &buf, &buf_size, &buf_allocated, 5000);
        if (!error) {

            sprintf(cmd, "+CPIN: READY\r\n\r\nSMS DONE\r\n\r\nPB DONE");
            if (!bistrstr(buf, buf_size, cmd, strlen(cmd))) {
                // PIN not accepted or SIM doesn't connect to network
                sprintf(cmd, "+CME ERROR: incorrect password");
                if (bistrstr(buf, buf_size, cmd, strlen(cmd))) {
                    // PIN not accepted
                    print_debug("TRsm", stderr, CYELLOW, 0, "Incorrect password!");
                } else {
                    // Error while setting PIN
                    print_debug("TRsm", stderr, CRED, 0, "PIN KO");
                    print_asbin(buf, buf_size);
                    print_ashex(buf, buf_size);
                }
                error = 1;
#if DEBUG_MODEM_SETUP
            } else {
                // PIN Ready, we can keep going
                print_debug("TRsm", stdout, CCYAN, 0, "PIN AUTH OK");
#endif
            }
        }
    }

    // Set CIPMODE
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CIPMODE=1");
#endif
        modem_cmd("AT+CIPMODE=1\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
    }

    // Start GPRS
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Start GPRS");
#endif

        // Go as usually
        error = !modem_cmd("AT+NETOPEN\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        if (!error) {
            if (bistrstr(buf, buf_size, "OK", 2)) {
                // Wait for new data
                buf_size=0;
                for (retry=10; retry>0; retry--) {
#if DEBUG_MODEM_SETUP
                    print_debug("TRsm", stdout, CBLUE, 0, "Waiting for link %d", retry);
#endif
                    if (!serial_recv(MODEM_SERIAL, &buf, &buf_size, &buf_allocated, 1000)) {
                        retry = 0;
                        print_debug("TRsm", stderr, CRED, 0, "Error while reading serial port");
                        break;
                    } else if (buf_size) {
                        // New data arrived
                        break;
                    }
                }

                // If we got out while some retries were left
                if (retry) {

                    // Check whatever we got
                    sprintf(cmd, "+NETOPEN: 0");
                    if (!bistrstr(buf, buf_size, cmd, strlen(cmd))) {
                        sprintf(cmd, "\r\n+IP ERROR: Network is already opened");
                        if (!bistrstr(buf, buf_size, cmd, strlen(cmd))) {
                            // Error while starting GPRS
                            print_debug("TRsm", stderr, CRED, 0, "NETOPEN has failed!");
                            print_asbin(buf, buf_size);
                            print_ashex(buf, buf_size);
                            error = 1;
                        }
                    }
                }
            } else {
                // Error while starting GPRS
                print_debug("TRsm", stderr, CRED, 0, "NETOPEN got unexpeced answer");
                print_asbin(buf, buf_size);
                print_ashex(buf, buf_size);
                error = 1;
            }
        }

    }

#endif

    // Get IPADDR
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Get IPDDR");
#endif
        error = !modem_cmd("AT+IPADDR\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        if (!error) {
            sprintf(cmd, "\r\n+IPADDR: ");
            if (!bistrcmp(cmd, strlen(cmd), buf, min(strlen(cmd), (unsigned int) buf_size))) {
                transmission_config.ipaddr[0] = 0;
                pointer=buf + strlen(cmd);
                for (i=0; i<(buf_size-strlen(cmd)); i++) {
                    if (*pointer=='\r') {
                        transmission_config.ipaddr[i] = 0;
                        break;
                    } else if (i==16) {
                        error = 1;
                        break;
                    } else {
                        transmission_config.ipaddr[i] = *pointer;
                        pointer++;
                    }
                }
                if (!error) {
                    if (i<16) {
#if DEBUG_MODEM_SETUP
                        print_debug("TRsm", stdout, CCYAN, 0, "Got IPDDR: %s", transmission_config.ipaddr);
#endif
                    } else {
                        print_debug("TRsm", stderr, CYELLOW, 0, "Couldn't read IPADDR!");
                        transmission_config.ipaddr[0] = 0;
                        print_asbin(buf, buf_size);
                        print_ashex(buf, buf_size);
                        error = 1;
                    }
                } else {
                    print_debug("TRsm", stderr, CYELLOW, 0, "IPADDR seems to be too long! (%d bytes is bigger than max expected 16 bytes)", buf_size - strlen(cmd));
                    print_asbin(buf, buf_size);
                    print_ashex(buf, buf_size);
                    transmission_config.ipaddr[0] = 0;
                    error = 1;
                }
            } else {
                print_debug("TRsm", stderr, CYELLOW, 0, "Didn't get IPADDR!");
                print_asbin(buf, buf_size);
                print_ashex(buf, buf_size);
                error = 1;
            }
        }

    }

    // Send our IPADDR
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Send our IPADDR");
#endif
        sprintf(cmd, "AT+CHTTPACT=\"%s\",%d\r\n", MANAGER_HOST, MANAGER_PORT);
        error = !modem_cmd(cmd, "\r\n+CHTTPACT: REQUEST", &buf, &buf_size, &buf_allocated, 2000);

        if (!error) {
            sprintf(cmd, "GET http://%s%s?name=%s&version=%s&build=%s&ip=%s&port=%d HTTP/1.1\r\nHost: %s\r\nUser-Agent: Alioli Buoy v%s\r\nAccept: text/html\r\nConnection: close\r\n\r\n", MANAGER_HOST, MANAGER_URI, SYSNAME, VERSION, BUILD_VERSION, transmission_config.ipaddr, EXTERNAL_PORT, MANAGER_HOST, VERSION);
            error = !modem_cmd(cmd, "\r\nOK", &buf, &buf_size, &buf_allocated, 2000);
            if (!error) {

                // Intialize
                pointer = bistrstr(buf, buf_size, "HTTP/1.1 200 OK", 15);
                if (pointer) {
                    pointer = bstrstr(pointer, buf_size-(pointer-buf), "\r\n\r\n", 4);
                    if (pointer) {
                        // Advance pointer to BODY position
                        pointer += 4;
                        pointer = bistrstr(pointer, buf_size-(pointer-buf), "OK", 2);
                        if (!pointer) {
                            print_debug("TRsm", stderr, CRED, 0, "UNEXPECTED HTTP ANSWER: remote system didn't answer with OK! :-(");
                            error = 1;
#if DEBUG_MODEM_SETUP
                        } else {
                            print_debug("TRsm", stdout, CCYAN, 0, "Our IP Address has been registered SUCESSFULLY");
#endif
                        }
                    } else {
                        print_debug("TRsm", stderr, CRED, 0, "INVALID HTTP: body not found!");
                        error = 1;
                    }
                } else {
                    print_debug("TRsm", stderr, CRED, 0, "INVALID HTTP: header not found or not \"HTTP/1.1 200 OK\"!");
                    error = 1;
                }

                /*
                if (!error) {
                    buf_size = 0;
                    while (!buf_size) {
#if DEBUG_MODEM_SETUP
                        print_debug("TRsm", stdout, CCYAN, 0, "    > Waiting for HTTP session to finish...");
#endif
                        serial_recv(MODEM_SERIAL, &buf, &buf_size, &buf_allocated, 1000);
                    }
                    if (!bistrstr(buf, buf_size, "+CHTTPACT: 0", 12)) {
                        print_debug("TRsm", stderr, CRED, 0, "Socket didn't finish tranmission");
                        error = 1;
                    }
                }*/

                // Show buffer if some error happened
                if (error) {
                    print_asbin(buf, buf_size);
                    print_ashex(buf, buf_size);
                }
            }

#if VERIFY_REMOTE_SERVER_ANSWER==0
            error = 0;
#endif
        }
    }

    // Open listening socket
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Open socket");
#endif

        // Go as usually
        sprintf(cmd, "AT+SERVERSTART=%d,0\r\n", EXTERNAL_PORT);
        if (modem_cmd(cmd, "\r\nOK", &buf, &buf_size, &buf_allocated, 500)) {
            print_debug(NULL, stdout, CWHITE, COLOR_NOHEAD_NOTAIL, "MODEM=%s:%d ", transmission_config.ipaddr, EXTERNAL_PORT);
            transmission_config.modem_ready = 1;
            transmission_config.modem_errors = 0;
        } else {
            error = 1;
        }
    }

    // Free memory
    free(cmd);
    if (buf) {
        free(buf);
    }

    return !error;
}

unsigned short int modem_getmsg(char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    unsigned short int error=0;

    // Initialize
    (*buf_size) = 0;


    error = !serial_recv(MODEM_SERIAL, buf, buf_size, buf_allocated, wait);

    return !error;
}

unsigned short int modem_gps(char **buf, size_t *buf_size, size_t *buf_allocated) {
    unsigned short int error=0;

    // Initialize
    (*buf_size) = 0;

    return !error;
}

unsigned short int modem_sendmsg(char *answer, size_t answer_size) {
    unsigned short int error=0;

    return serial_send(MODEM_SERIAL, answer, answer_size);
}
#endif

#ifdef RS485_SERIAL
unsigned short int rs485_getmsg(char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    unsigned short int error=0;

    // Initialize
    (*buf_size) = 0;


    error = !serial_recv(RS485_SERIAL, buf, buf_size, buf_allocated, wait);

    return !error;
}

unsigned short int rs485_sendmsg(char *buf, size_t buf_size) {
    return serial_send(RS485_SERIAL, buf, buf_size);
}
unsigned short int rs485_cmd(const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    return serial_cmd(RS485_SERIAL, "TR-R", cmd, expected, buf, buf_size, buf_allocated, wait);
}

unsigned short int rs485_setup() {
    unsigned short int error=0;
    char *buf=NULL;
    size_t buf_size=0, buf_allocated=0;

    char temp[40]="";

    // Initialize
    transmission_config.rs485_ready = 0;
    transmission_config.rs485_ready = 1;
    transmission_config.rs485_errors = 0;

    // Connect link to ROV
    sprintf(temp, "PING - %d %d\r\n", millis(), SERIAL2_SPEED);
    // sprintf(temp, "%d - CONNECT\r\n", millis());
    if (rs485_cmd(temp, "PONG", &buf, &buf_size, &buf_allocated, 2000)) {
#if DEBUG_TRANSMISSION
        print_debug("TRsr", stdout, CCYAN, COLOR_NOHEAD_NOTAIL, "RS485 ");
#endif
        transmission_config.rs485_ready = 1;
        transmission_config.rs485_errors = 0;
    } else {
        error = 1;
    }

    // Free memory
    if (buf) {
        free(buf);
    }

    return !error;
}
#endif

void transmission_setup(long int now) {
#if DEBUG_TRANSMISSION
    print_debug("TRs", stdout, CPURPLE, COLOR_NOTAIL, "INI -> ");
#endif

    // Setup serial ports
    serial_setup();
    communication_setup();

    // Set local config
    transmission_config.nextevent = 0;
    transmission_config.gps_nextevent = 0;
    transmission_config.modem_ready = 0;
    transmission_config.modem_errors = 0;
    transmission_config.modem_linked = 0;
    transmission_config.rs485_ready = 0;
    transmission_config.rs485_errors = 0;

    // Setup devices
#ifdef RS485_SERIAL
    rs485_setup();
#else
    transmission_config.rs485_ready = 1;
#endif
#ifdef MODEM_SERIAL
    modem_setup();
#else
    transmission_config.modem_ready = 1;
#endif

#if DEBUG_TRANSMISSION
    // Show we are done
    print_debug("TRs", stdout, CGREEN, COLOR_NOHEAD, "DONE");
#endif
}

void transmission_loop(long int now) {
    int incomingByte = 0;
    int idx=0;
    char *buf=NULL, *answer=NULL, *pivot=NULL;
    unsigned int buf_size=0, buf_allocated=0, answer_size=0, answer_allocated=0;
    unsigned short int modem_error=0, rs485_error=0, bypass_msg=0;

    // Check transmission lookup
    if (transmission_config.nextevent<now) {

        // Set next event
        transmission_config.nextevent = now+TRANSMISSION_MS;

        // Check if MODEM is ready
        if (transmission_config.modem_ready && transmission_config.rs485_ready) {

#ifdef MODEM_SERIAL

            // === GET REQUEST FROM BASE === ==================================

            // Get remote MSG if any (and leave client waiting)
            buf_size = 0;
            modem_error = !modem_getmsg(&buf, &buf_size, &buf_allocated, TIMEOUT_MODEM_MS);

            // Check if there is a message to process
            if (buf_size) {

                // Message detected
#if DEBUG_TRANSMISSION_MSG
                print_debug("TRl", stdout, CPURPLE, 0, "MSG:");
                print_asbin(buf, buf_size);
                print_ashex(buf, buf_size);
#endif

                // CONNECT msg
                if (
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 7), "CONNECT", 7))
                    ||
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 9), "\r\nCONNECT", 9))
                ) {
                    strcat_realloc(&answer, &answer_size, &answer_allocated, "WELCOME\n", 8, __FILE__, __LINE__);
#if DEBUG_TRANSMISSION
                    print_debug("TRl", stdout, CCYAN, 0, "LINK OPEN");
#endif
                    buf_size = 0;
                    transmission_config.modem_linked = 1;
                    communication_reset();

                // CLOSED msg
                } else if (
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 6), "CLOSED", 6))
                    ||
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 8), "\r\nCLOSED", 8))
                ) {
#if DEBUG_TRANSMISSION
                    print_debug("TRl", stdout, CBLUE, 0, "LINK CLOSED");
#endif
                    buf_size = 0;
                    transmission_config.modem_linked = 0;
                    communication_reset();

                // HELLO msg
                } else if (
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 5), "PING", 5))
                    ||
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 6), "\nPING", 6))
                    ||
                    (!bistrcmp(buf, min(buf_size, (unsigned int) 7), "\r\nPING", 7))
                ) {
#if DEBUG_TRANSMISSION
                    print_debug("TRl", stdout, CCYAN, 0, "LINK PING");
#endif
                    buf_size = 0;
                    strcat_realloc(&answer, &answer_size, &answer_allocated, "PONG\n", 5, __FILE__, __LINE__);
                    transmission_config.modem_linked = 0;
                    communication_reset();

                // MAVLINK msg
                } else if (transmission_config.modem_linked) {
                    // Process message locally if linked to external system
#if DEBUG_TRANSMISSION_MSG
                    print_debug("TRl", stdout, CCYAN, 0, "LINK MAVLINK");
#endif
                    // remote_msg(&buf, &buf_size, &buf_allocated, &answer, &answer_size, &answer_allocated);
                } else {
                    // Check connection to the GSM module
                    modem_error = !modem_cmd("AT\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
                    if (!modem_error) {
                        if (!bistrstr(buf, buf_size, "OK", 2)) {
                            // No answer to PIN status
                            print_debug("TRl", stderr, CRED, 0, "AT command has failed!");
                            print_asbin(buf, buf_size);
                            print_ashex(buf, buf_size);
                            modem_error = 1;
                        }
                    }
                }

            }

#endif

#ifdef RS485_SERIAL

            // === ASK ROV === ================================================

            // If it is a meesage for the ROV
            if (buf_size) {

                // Send message to RS485
                if (rs485_sendmsg(buf, buf_size)) {

                    // Get answer
                    buf_size = 0;
                    if (rs485_getmsg(&buf, &buf_size, &buf_allocated, TIMEOUT_RS485_MS)) {

                        // Check if there is a message to process
                        if (buf_size) {

                            // Analize answer from rov
                            // rov_msg(buf, buf_size, &answer, &answer_size, &answer_allocated);

                            // RS485 data found
                            print_debug("TRl", stdout, CPURPLE, 0, "RS485:");
                            print_asbin(answer, answer_size);
                            print_ashex(answer, answer_size);

                        }
                    } else {
                        rs485_error = 1;
                    }
                } else {
                    rs485_error = 1;
                }
            }
#endif

#ifdef MODEM_SERIAL

            // === SEND ANSWER === ============================================

            // SEND answer to BASE
            if (answer_size) {

                // Send answer to modem
                modem_error = !modem_sendmsg(answer, answer_size);

            }

#endif

            // Free memory
            if (answer) {
                free(answer);
                answer = NULL;
                answer_size = 0;
            }

            // === ERROR CHECKING === =====================================
#ifdef MODEM_SERIAL
            // Check for errors in MODEM
            if (!modem_error) {

                // There was no error, count one error less
                if (transmission_config.modem_errors) {
                    transmission_config.modem_errors--;
                }

            } else {
                // There was an error, count one error more
                transmission_config.modem_errors++;
            }
#endif

#ifdef RS485_SERIAL
            // Check for errors in RS485
            if (!rs485_error) {

                // There was no error, count one error less
                if (transmission_config.rs485_errors) {
                    transmission_config.rs485_errors--;
                }

            } else {
                // There was an error, count one error more
                transmission_config.modem_errors++;
            }
#endif

#if DEBUG_TRANSMISSION
//        print_debug("TR", stdout, CPURPLE, 0, "Actual MODEM errors: %d", transmission_config.modem_errors);
//        print_debug("TR", stdout, CPURPLE, 0, "Actual RS485 errors: %d", transmission_config.rs485_errors);
#endif

#ifdef MODEM_SERIAL
            // Check if we should RESET the MODEM (too many errors)
            if (transmission_config.modem_errors>RESET_MODEM_AFTER_ERRORS) {
                transmission_config.modem_ready = 0;
            }
#endif

#ifdef RS485_SERIAL
            // Check if we should RESET the RS485 (too many errors)
            if (transmission_config.rs485_errors>RESET_RS485_AFTER_ERRORS) {
                transmission_config.rs485_ready = 0;
            }
#endif

        }

        // === TRY TO RECONNECT IF NOT CONNECTED === ======================
#ifdef MODEM_SERIAL
        if (!transmission_config.modem_ready) {
            // Try to initialize the MODEM
            modem_setup();
        }
#endif

#ifdef RS485_SERIAL
        if (!transmission_config.rs485_ready) {
            // Try to initialize the MODEM
            rs485_setup();
        }
#endif

    }

    /*
    // === GET GPS POSITION ===
    if ((transmission_config.modem_ready) && (transmission_config.gps_nextevent<now)) {

        // Set next event
        transmission_config.gps_nextevent = now+TRANSMISSION_GPS_MS;

        // Get GPS position and timing
        buf_size = 0;
        modem_error = !modem_gps(&buf, &buf_size, &buf_allocated);

        // Check if there is a message to process
        if (buf_size) {

            // GPS data found
            print_debug("TRl", stdout, CPURPLE, 0, "GPS:");
            print_asbin(buf, buf_size);
            print_ashex(buf, buf_size);

        }

    }
   */

    // Free memory
    if (buf) {
        free(buf);
    }

    // Loop control
    yield();
    delay(10);

}
