// #include "lib/shared.h"
// #include "lib/config.h"
#include "lib/version.h"
#include "lib/common/alioli.h"
#include "lib/common/serial.h"
#include "lib/common/protocol.h"

#include "transmission.h"
#include "communication.h"

TransmissionConfig transmission_config;

void tranmission_switch_led(unsigned short int *value) {
    if (*value) {
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    (*value) = !(*value);
}

#ifdef MODEM_SERIAL
unsigned short int modem_cmd(const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned long int wait, unsigned long int wait_transfer) {
    return serial_cmd(MODEM_SERIAL, "TR-M", cmd, expected, buf, buf_size, buf_allocated, wait, wait_transfer, 0);
}

unsigned short int modem_getmsg(char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait, unsigned int wait_transfer) {
    unsigned short int error=0;

    // Initialize
    (*buf_size) = 0;


    error = !serial_recv(MODEM_SERIAL, buf, buf_size, buf_allocated, wait, wait_transfer, 0);

    return !error;
}

unsigned short int modem_send_our_ip(char **buf, size_t *buf_size, size_t *buf_allocated) {
    char cmd[400]="", *pointer=NULL;
    unsigned short int error=0;

    // Send our IPADDR
    sprintf(cmd, "AT+CHTTPACT=\"%s\",%d\r\n", MANAGER_HOST, MANAGER_PORT);
    error = !modem_cmd(cmd, "+CHTTPACT: REQUEST", buf, buf_size, buf_allocated, 2000, 0);

    if (!error) {
        sprintf(cmd, "GET http://%s%s?name=%s&version=%s&build=%s&ip=%s&port=%d HTTP/1.1\r\nHost: %s\r\nUser-Agent: Alioli Buoy v%s\r\nAlioli-Key: %s\r\nAccept: text/html\r\nConnection: close\r\n\r\n", MANAGER_HOST, MANAGER_URI, SYSNAME, VERSION, BUILD_VERSION, transmission_config.ipaddr, EXTERNAL_PORT, MANAGER_HOST, VERSION, HTTP_KEY);
        // print_debug("TRm", stdout, CBLUE, 0, "> %s", cmd);
        error = !modem_cmd(cmd, "OK", buf, buf_size, buf_allocated, 2000, 0);
        if (!error) {

            // Check if header already came up
            if (!bistrstr(*buf, *buf_size, "HTTP/1.1 200 OK", 15)) {
                // Wait for the server to answer
                error = !modem_cmd(cmd, "+CHTTPACT:", buf, buf_size, buf_allocated, 2000, 0);
            }

            // Initialize
            pointer = bistrstr(*buf, *buf_size, "HTTP/1.1 200 OK", 15);
            if (pointer) {
                pointer = bstrstr(pointer, *buf_size-(pointer-*buf), "\r\n\r\n", 4);
                if (pointer) {
                    // Advance pointer to BODY position
                    pointer += 4;
                    pointer = bistrstr(pointer, *buf_size-(pointer-*buf), "OK", 2);
                    if (!pointer) {
                        print_debug("TRsm", stderr, CRED, 0, "UNEXPECTED HTTP ANSWER: remote system didn't answer with OK! :-(");
                        error = 1;
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
                *buf_size = 0;
                while (!*buf_size) {
#if DEBUG_MODEM_SETUP
                    print_debug("TRsm", stdout, CCYAN, 0, "    > Waiting for HTTP session to finish...");
#endif
                    serial_recv(MODEM_SERIAL, buf, buf_size, buf_allocated, 1000, 500, 0);
                }
                if (!bistrstr(*buf, *buf_size, "+CHTTPACT: 0", 12)) {
                    print_debug("TRsm", stderr, CRED, 0, "Socket didn't finish tranmission");
                    error = 1;
                }
            }*/

            // Show buffer if some error happened
            if (error) {
                print_asbin(*buf, *buf_size, stderr);
                print_ashex(*buf, *buf_size, stderr);
            }
        }

#if VERIFY_REMOTE_SERVER_ANSWER==0
        error = 0;
#endif
    }

    // Return if some error happened
    return error;
}

unsigned short int modem_setup() {
    char *buf=NULL, *cmd=NULL, *pointer=NULL;
    size_t buf_size=0, buf_allocated=0;
    unsigned short int error=0, pin_ready=0, i=0, retry=0, led=0;

    // Prepare cmd
    cmd = (char*) malloc(sizeof(char)*400);
    cmd[0]=0;

    // Reset modem status
    transmission_config.modem_ready = 0;
    transmission_config.modem_linked = 0;

    // Show information
    tranmission_switch_led(&led);
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
        // Make sure we are online
        tranmission_switch_led(&led);
        if (!modem_cmd("+++", NULL, &buf, &buf_size, &buf_allocated, 1000, 1000)) {
            // Try a second time
            tranmission_switch_led(&led);
            modem_cmd("+++", NULL, &buf, &buf_size, &buf_allocated, 1000, 1000);
        }
    }

    // Do a full RESET from the MODULE before starting
    tranmission_switch_led(&led);
    modem_cmd("AT+CRESET\r\n", "OK", &buf, &buf_size, &buf_allocated, 1000, 1000);
    if (!bistrstr(buf, buf_size, "RDY", 3)) {

        // Wait for new data
        buf_size=0;
        for (retry=5; retry>0; retry--) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "Waiting for module to RESET %d", retry);
#endif
            tranmission_switch_led(&led);
            if (serial_recv(MODEM_SERIAL, &buf, &buf_size, &buf_allocated, 10000, 1000, 0)) {
                if (bistrstr(buf, buf_size, "RDY", 3)) {
                    break;
                }
            } else {
                retry = 0;
                print_debug("TRsm", stderr, CRED, 0, "Error while reading Modem serial port");
                break;
            }
        }
        if (!retry) {
            print_debug("TRsm", stderr, CRED, 0, "Error while reading Modem serial port");
        }
    }

#if MODEM_SIMULATED==0
    // Get back from any process
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CTRL+Z");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("", NULL, &buf, &buf_size, &buf_allocated, 500, 0);
    }
#endif

    // Reset modem
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "RESET");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("ATZ\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 0);
    }

    // Remove ECHO
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "ECHO OFF");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("ATE0\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000, 100);
        // Empty buffer
        tranmission_switch_led(&led);
        modem_getmsg(&buf, &buf_size, &buf_allocated, 0, 0);
    }

    // Check AT
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CHECK AT");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("AT\r\n", "OK", &buf, &buf_size, &buf_allocated, 500, 0);
        if (!error) {
            if (!bistrstr(buf, buf_size, "OK", 2)) {
                // No answer to PIN status
                print_debug("TRsm", stderr, CRED, 0, "AT command has failed!");
                if (buf_size) {
                    print_asbin(buf, buf_size, stderr);
                    print_ashex(buf, buf_size, stderr);
                }
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
        tranmission_switch_led(&led);
        error = !modem_cmd("AT+CFUN=6\r\n", "OK", &buf, &buf_size, &buf_allocated, 10000, 0);
    }

    // Startup Module
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Enable MODULE (15 sec)");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("AT+CFUN=1\r\n", "OK", &buf, &buf_size, &buf_allocated, 15000, 0);
    }

    // Reset modem
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "RESET");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("ATZ\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 500);
    }

    // Remove ECHO
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "ECHO OFF");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("ATE0\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000, 100);
    }

    // Check AT
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CHECK AT");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("AT\r\n", "OK", &buf, &buf_size, &buf_allocated, 500, 0);
    }

    // Check PIN status
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CHECK PIN STATUS");
#endif
        tranmission_switch_led(&led);
        error = !modem_cmd("AT+CPIN?\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000, 0);
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
                    print_debug("TRsm", stderr, CRED, 0, "NO PING REQUESTED");
                    print_asbin(buf, buf_size, stderr);
                    print_ashex(buf, buf_size, stderr);
                    error = 1;
#if DEBUG_MODEM_SETUP
                } else {
                    print_debug("TRsm", stdout, CCYAN, 0, "PIN REQUESTED");
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
        tranmission_switch_led(&led);
        error = !modem_cmd(cmd, NULL, &buf, &buf_size, &buf_allocated, 5000, 2000);
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
                    print_asbin(buf, buf_size, stderr);
                    print_ashex(buf, buf_size, stderr);
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

    // Delete ALL SMS
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "AT+CMGD=1,4  (Delete ALL SMSs)");
#endif
        tranmission_switch_led(&led);
        modem_cmd("AT+CMGD=1,4\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 500);
    }

    // Set CIPMODE
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "CIPMODE=1");
#endif
        tranmission_switch_led(&led);
        modem_cmd("AT+CIPMODE=1\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 500);
    }

    // Start GPRS
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Start GPRS");
#endif

        // Go as usually
        tranmission_switch_led(&led);
        error = !modem_cmd("AT+NETOPEN\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 500);
        if (!error) {
            // Check if OK is in the answer
            if (bistrstr(buf, buf_size, "OK", 2)) {

                // Check if NETOPEN was also answered
                sprintf(cmd, "+NETOPEN: 0");
                if (!bistrstr(buf, buf_size, cmd, strlen(cmd))) {

                    sprintf(cmd, "+IP ERROR: Network is already opened");
                    if (!bistrstr(buf, buf_size, cmd, strlen(cmd))) {

                        // Wait for new data
                        buf_size=0;
                        for (retry=10; retry>0; retry--) {
#if DEBUG_MODEM_SETUP
                            print_debug("TRsm", stdout, CBLUE, 0, "Waiting for link %d", retry);
#endif
                            tranmission_switch_led(&led);
                            if (!serial_recv(MODEM_SERIAL, &buf, &buf_size, &buf_allocated, 1000, 500, 0)) {
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
                                    print_asbin(buf, buf_size, stderr);
                                    print_ashex(buf, buf_size, stderr);
                                    error = 1;
                                }
                            }

                        }
                    }
                }

            } else {
                // Error while starting GPRS
                print_debug("TRsm", stderr, CRED, 0, "NETOPEN got unexpeced answer");
                print_asbin(buf, buf_size, stderr);
                print_ashex(buf, buf_size, stderr);
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
        tranmission_switch_led(&led);
        error = !modem_cmd("AT+IPADDR\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 0);
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
                        print_asbin(buf, buf_size, stderr);
                        print_ashex(buf, buf_size, stderr);
                        error = 1;
                    }
                } else {
                    print_debug("TRsm", stderr, CYELLOW, 0, "IPADDR seems to be too long! (%d bytes is bigger than max expected 16 bytes)", buf_size - strlen(cmd));
                    print_asbin(buf, buf_size, stderr);
                    print_ashex(buf, buf_size, stderr);
                    transmission_config.ipaddr[0] = 0;
                    error = 1;
                }
            } else {
                print_debug("TRsm", stderr, CYELLOW, 0, "Didn't get IPADDR!");
                print_asbin(buf, buf_size, stderr);
                print_ashex(buf, buf_size, stderr);
                error = 1;
            }
        }

    }

    // Send our IPADDR
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRm", stdout, CBLUE, 0, "Send our IPADDR");
#endif
        tranmission_switch_led(&led);
        error = modem_send_our_ip(&buf, &buf_size, &buf_allocated);
#if DEBUG_MODEM_SETUP
        if (!error) {
            print_debug("TRsm", stdout, CCYAN, 0, "Our IP Address has been registered SUCESSFULLY");
        }
#endif
    }

    // Open listening socket
    if (!error) {
#if DEBUG_MODEM_SETUP
        print_debug("TRsm", stdout, CBLUE, 0, "Open socket");
#endif

        // Go as usually
        sprintf(cmd, "AT+SERVERSTART=%d,0\r\n", EXTERNAL_PORT);
        tranmission_switch_led(&led);
        if (modem_cmd(cmd, "OK", &buf, &buf_size, &buf_allocated, 2000, 500)) {
            // Verify that server is UP
            sprintf(cmd, "+SERVERSTART: 0,%d", EXTERNAL_PORT);
            tranmission_switch_led(&led);
            if (modem_cmd("AT+SERVERSTART?\r\n", cmd, &buf, &buf_size, &buf_allocated, 500, 0)) {
                // Server is UP
                print_debug(NULL, stdout, CWHITE, COLOR_NOHEAD_NOTAIL, "MODEM=%s:%d ", transmission_config.ipaddr, EXTERNAL_PORT);
                transmission_config.modem_ready = 1;
                transmission_config.modem_errors = 0;
            }
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

unsigned short int modem_gps(char **buf, size_t *buf_size, size_t *buf_allocated) {
    int i=0;
    unsigned short int error=0;
    char **tokens=NULL, *token=NULL, temp[3]={0, 0, 0};
    unsigned int year=0;
    unsigned short int month=0, day=0, hour=0, minute=0, second=0;

    // Initialize
    (*buf_size) = 0;

    // Request GPS position
    error = !modem_cmd("AT+CGPSINFO\r\n", "+CGPSINFO: ", buf, buf_size, buf_allocated, 500, 0);
    if (!error) {
#if DUMMY_GPS
        print_debug("TRgps", stdout, CYELLOW, 0, "Using DUMMY GPS! (29.4 -13.51 | 29.4N 13.51W)");
#if DEBUG_TRANSMISSION
        // Show what we got
        // print_asbin(*buf, *buf_size, stderr);
        // print_ashex(*buf, *buf_size, stderr);
#endif
        // Set DUMMY GPS position
        (*buf_size) = 0;
        // Example: 29.4 -13.51 (29.5N 13.51W)
        strcat_realloc(buf, buf_size, buf_allocated, "+CGPSINFO: 2940.000000,N,01351.000000,W,220822,171746.0,23.1,0.0,0.0", 68, __FILE__, __LINE__);
#endif

        // Convert positions to decimal degrees (N=+ S=- E=+ W=-) - [ len("+CGPSINFO: ")==11 ]
        tokens = bstr_split((*buf)+11, (*buf_size)-11, ',', (char*) __FILE__, __LINE__);
        if (tokens) {
            buoy.gps_newdata = 1;
            for (i=0; *(tokens+i); i++) {
                token = *(tokens+i);
                if (i==0) {
                    // Latitude
                    if (*token) {
                        buoy.gps.latitude = atof(token) / 100;
                    } else {
                        buoy.gps.latitude = 0.0;
                    }
                } else if (i==1) {
                    // Latitude sign
                    if ((*token) && (*token=='S')) {
                        buoy.gps.latitude *= -1;
                    }
                } else if (i==2) {
                    // Longitude
                    if (*token) {
                        buoy.gps.longitude = atof(token) / 100;
                    } else {
                        buoy.gps.longitude = 0.0;
                    }
                } else if (i==3) {
                    // Longitude sign
                    if ((*token) && (*token=='W')) {
                        buoy.gps.longitude *= -1;
                    }
                } else if (i==4) {
                    // UTC Date
                    if (*token) {
                        temp[0] = token[0];
                        temp[1] = token[1];
                        year = atoi(temp);
                        temp[0] = token[2];
                        temp[1] = token[3];
                        month = atoi(temp);
                        temp[0] = token[4];
                        temp[1] = token[5];
                        day = atoi(temp);
                    } else {
                        year = 0;
                        month = 0;
                        day = 0;
                    }
                } else if (i==5) {
                    // UTC Time
                    if (*token) {
                        temp[0] = token[0];
                        temp[1] = token[1];
                        hour = atoi(temp);
                        temp[0] = token[2];
                        temp[1] = token[3];
                        minute = atoi(temp);
                        temp[0] = token[4];
                        temp[1] = token[5];
                        second = atoi(temp);
                    } else {
                        hour = 0;
                        minute = 0;
                        second = 0;
                    }
                } else if (i==6) {
                    // Altitude
                    if (*token) {
                        buoy.gps.altitude = atof(token);
                    } else {
                        buoy.gps.altitude = 0.0;
                    }
                } else if (i==7) {
                    // Speed
                    if (*token) {
                        buoy.gps.speed = atof(token);
                    } else {
                        buoy.gps.speed = 0.0;
                    }
                } else if (i==8) {
                    // Course
                    if (*token) {
                        buoy.gps.course = atof(token);
                    } else {
                        buoy.gps.course = 0.0;
                    }
                }
            }
            // Free tokens
            free(tokens);
            // Convert date
            if (year>0) {
                buoy.gps.epoch = date2epoch(year, month, day, hour, minute, second);
            } else {
                buoy.gps.epoch = 0;
            }
        }
    }

    return !error;
}

unsigned short int modem_sendmsg(char *answer, size_t answer_size) {
    return serial_send(MODEM_SERIAL, answer, answer_size);
}
#endif

#ifdef RS485_SERIAL
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
unsigned short int rs485_cmd(const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned long int wait) {
    return serial_cmd(RS485_SERIAL, "TR-R", cmd, expected, buf, buf_size, buf_allocated, wait, 0, 0);
}

unsigned short int rs485_setup() {
    unsigned short int error=0;
    char *buf=NULL;
    size_t buf_size=0, buf_allocated=0;

    // char temp[40]="";

    // Initialize
    transmission_config.rs485_ready = 0;
    transmission_config.rs485_errors = 0;

    // Connect link to ROV
    // sprintf(temp, "PING - %lu %d\r\n", millis(), SERIAL2_SPEED);
    // sprintf(temp, "%d - CONNECT\r\n", millis());
    if (rs485_cmd("PING", "PONG", &buf, &buf_size, &buf_allocated, 2000)) {
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

    // Setup modules
    communication_setup();

    // Set local config
    transmission_config.nextevent = 0;
    transmission_config.gps_nextevent = 0;
    transmission_config.ourip_nextevent = 0;
    transmission_config.webserver_nextevent = 0;
    transmission_config.ping_rov_nextevent = 0;
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
    delay(2000);
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
    char *buf=NULL, *answer=NULL;
    unsigned int buf_size=0, buf_allocated=0, answer_size=0, answer_allocated=0;
    unsigned short int modem_error=0, modem_action=0, rs485_error=0, rs485_action=0;
    HeartBeat heartbeat;


    // Check transmission lookup
    if (transmission_config.nextevent<now) {

#if DEBUG_TRANSMISSION
        // print_debug("TRl", stdout, CGREEN, 0, "ALIVE (MODEM errors: %d, RS485 errors: %d)", transmission_config.modem_errors, transmission_config.rs485_errors);
#endif

        // Set next event
        transmission_config.nextevent = now+TRANSMISSION_MS;

        // Check if MODEM is ready
        if (transmission_config.modem_ready && transmission_config.rs485_ready) {

#ifdef MODEM_SERIAL

            // === GET REQUEST FROM BASE === ==================================

            // Read from buffer if no other task must to be done
            buf_size = 0;
            if (
                   (transmission_config.gps_nextevent>=now)
                && (transmission_config.webserver_nextevent>=now)
                && (transmission_config.ourip_nextevent>=now)
            ) {
                // Get remote MSG if any (and leave client waiting)
                modem_error = !modem_getmsg(&buf, &buf_size, &buf_allocated, TIMEOUT_MODEM_MS, TRANSFER_MODEM_WAIT_MS);
            }

            // If no error
            if (!modem_error) {

                // Check if there is a message to process
                if (buf_size) {

                    // Message detected
#if DEBUG_TRANSMISSION
                    print_debug("TRl", stdout, CPURPLE, 0, "MODEM->BUOY msg with %ld bytes", buf_size);
                    print_asbin(buf, buf_size, stderr);
                    print_ashex(buf, buf_size, stderr);
#endif

                    // CONNECT msg
                    if (
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 7), "+CLIENT", 7))
                        ||
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 9), "\r\n+CLIENT", 9))
                    ) {
                        strcat_realloc(&answer, &answer_size, &answer_allocated, "WELCOME\n", 8, __FILE__, __LINE__);
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "LINK OPEN");
#endif
                        transmission_config.modem_linked = 1;
                        communication_reset();
                        buf_size = 0;

                    // CLOSED msg
                    } else if (
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 6), "CLOSED", 6))
                        ||
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 8), "\r\nCLOSED", 8))
                    ) {
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CBLUE, 0, "LINK CLOSED");
#endif
                        transmission_config.modem_linked = 0;
                        communication_reset();
                        buf_size = 0;

                    // CMTI message (there is a message waiting)
                    } else if (
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 6), "+CMTI", 6))
                        ||
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 7), "\n+CMTI", 7))
                        ||
                        (!bistrcmp(buf, min(buf_size, (unsigned int) 8), "\r\n+CMTI", 8))
                    ) {
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CBLUE, 0, "SMS waiting");
#endif
                        buf_size = 0;

/*
                    // This message is outdated (use onlye with ModemSimul)
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
                        buf_size = 0;
*/

                    // MAVLINK msg
                    } else if (transmission_config.modem_linked) {
                        // Process message locally if linked to external system
#if DEBUG_TRANSMISSION_MSG
                        print_debug("TRl", stdout, CCYAN, 0, "LINK MAVLINK");
#endif
                        remote_msg(&buf, &buf_size, &buf_allocated, &answer, &answer_size, &answer_allocated);
                    } else {
                        // Check connection to the GSM module
                        modem_action = 1;
                        modem_error = !modem_cmd("AT\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 100);
                        if (!modem_error) {
                            if (!bistrstr(buf, buf_size, "OK", 2)) {
                                // No answer to Modem status
                                print_debug("TRl", stderr, CRED, 0, "AT command has failed!");
                                print_asbin(buf, buf_size, stderr);
                                print_ashex(buf, buf_size, stderr);
                                modem_error = 1;
                            }
                        }
                        buf_size = 0;
                    }

                // No data in the BUS, check if there is work to do
                } else if (
                           (transmission_config.gps_nextevent<now)
                        || (transmission_config.webserver_nextevent<now)
                        || (transmission_config.ourip_nextevent<now)
                    ) {

                    // This requires modem_actions

                    // Get back to AT mode
                    /*
                    if (transmission_config.modem_linked) {
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "Go to AT MODE");
#endif
                        // modem_error = !modem_cmd("+++", "OK", &buf, &buf_size, &buf_allocated, 0, 1000);
                        serial_cmd(MODEM_SERIAL, "TRl", "+++", NULL, &buf, &buf_size, &buf_allocated, 0, 1000, 0);
                    }
                    */
                    serial_cmd(MODEM_SERIAL, "TRl", "+++", NULL, &buf, &buf_size, &buf_allocated, 0, 1000, 0);

                    // === SEND OUR IP ADDRESS ===
                    if ((!modem_error) && (transmission_config.ourip_nextevent<now)) {
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "Send our IP ADDRESS");
#endif
                        // Set next event
                        transmission_config.ourip_nextevent = now+TRANSMISSION_OURIP_MS;

                        // Send our IP
                        modem_action = 1;
                        modem_error = modem_send_our_ip(&buf, &buf_size, &buf_allocated);

#if DEBUG_MODEM_SETUP
                        if (!modem_error) {
                            print_debug("TRl", stdout, CCYAN, 0, "Our IP Address has been registered SUCESSFULLY");
                        }
#endif
                    }

                    // === GET WEBSERVER STATUS ===
                    if ((!modem_error) && (transmission_config.webserver_nextevent<now)) {

#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "CHECK WEBSERVER STATUS");
#endif

                        // Set next event
                        transmission_config.webserver_nextevent = now+TRANSMISSION_WEBSERVER_MS;

                        // Check connection to the GSM module
                        modem_action = 1;
                        modem_error = !modem_cmd("AT+SERVERSTART?\r\n", "+SERVERSTART: 0,", &buf, &buf_size, &buf_allocated, 100, 0);
                        if (modem_error && (!bistrstr(buf, buf_size, "+CIPEVENT: NETWORK CLOSED UNEXPECTEDLY", 38))) {
                            modem_cmd("AT+NETCLOSE\r\n", NULL, &buf, &buf_size, &buf_allocated, 500, 0);
                        }
                    }

                    // === GET GPS POSITION ===
                    if ((!modem_error) && (transmission_config.gps_nextevent<now)) {

#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "REQUEST GPS POSITION");
#endif

                        // Set next event
                        transmission_config.gps_nextevent = now+TRANSMISSION_GPS_MS;

                        // Get GPS position and timing
                        buf_size = 0;
                        modem_action = 1;
                        modem_error = !modem_gps(&buf, &buf_size, &buf_allocated);
                    }

                    // Go back to DATA mode
                    if (!modem_error && transmission_config.modem_linked) {
#if DEBUG_TRANSMISSION
                        print_debug("TRl", stdout, CCYAN, 0, "Go back to DATA MODE");
#endif
                        serial_cmd(MODEM_SERIAL, "TRl", "ATO\r\n", "CONNECT", &buf, &buf_size, &buf_allocated, 100, 0, 1);

                        // Connection has been closed, we are not linked anymore
                        if (bistrstr(buf, buf_size, "NO CARRIER", 10)) {
                            transmission_config.modem_linked = 0;
#if DEBUG_TRANSMISSION
                            print_debug("TRl", stdout, CRED, 0, "LINK LOST!");
#endif
                        }

                    }

                    buf_size = 0;
                }
            }

#endif

#ifdef RS485_SERIAL

            // === ASK ROV === ================================================

            // If we should send a PING, attach to to the buffer
            if (transmission_config.ping_rov_nextevent<now) {

#if DEBUG_TRANSMISSION
                print_debug("TRl", stdout, CCYAN, 0, "Query for ROV's Heartbeat");
#endif

                // Prepare for next ping
                transmission_config.ping_rov_nextevent = now+TRANSMISSION_PING_ROV_MS;

                // Set our timestamp
                heartbeat.requested = get_current_time();
                heartbeat.answered = 0;

                // Pack
                protocol_pack_heartbeat(&heartbeat, protocol_counter, &buf, &buf_size, &buf_allocated);
                if (protocol_counter==255) {
                    protocol_counter = 1;
                } else {
                    protocol_counter++;
                }
            }

            // If it is a meesage for the ROV
            if (buf_size) {

                // Message detected
#if DEBUG_TRANSMISSION_MSG
                print_debug("TRl", stdout, CPURPLE, 0, "BUOY->ROV msg with %ld bytes", buf_size);
                // print_asbin(buf, buf_size, stderr);
                print_ashex(buf, buf_size, stderr);
#endif

                // This is a RS485 action
                rs485_action = 1;

                // Send message to RS485
                rs485_error = !rs485_sendmsg(buf, buf_size);
                if (!rs485_error) {

                    // Get answer
                    buf_size = 0;
                    rs485_error = !rs485_getmsg(&buf, &buf_size, &buf_allocated, TIMEOUT_RS485_MS, TRANSFER_RS485_WAIT_MS);
                    if (!rs485_error) {

                        // Check if there is a message to process
                        if (buf_size) {

                            // Message detected
#if DEBUG_TRANSMISSION_MSG
                            print_debug("TRl", stdout, CPURPLE, 0, "ROV->BUOY msg with %ld bytes", buf_size);
                            // print_asbin(buf, buf_size, stderr);
                            print_ashex(buf, buf_size, stderr);
#endif

                            // Analize answer from rov
                            rov_msg(buf, buf_size, &answer, &answer_size, &answer_allocated);

                        }
                    }
                }
            }
#endif

#ifdef MODEM_SERIAL

            // === SEND ANSWER === ============================================

            // SEND answer to BASE
            if (answer_size) {

                // RS485 answer found
#if DEBUG_TRANSMISSION_MSG
                print_debug("TRl", stdout, CPURPLE, 0, "BUOY->MODEM msg with %ld bytes", answer_size);
                // print_asbin(answer, answer_size, stderr);
                print_ashex(answer, answer_size, stderr);
#endif

                // Send answer to modem
                modem_action = 1;
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
            if (modem_action) {
                if (!modem_error) {

                    // There was no error, count one error less
                    if (transmission_config.modem_errors) {
                        transmission_config.modem_errors--;
                    }

                } else {
                    // There was an error, count one error more
                    transmission_config.modem_errors++;
                }
            }
#endif

#ifdef RS485_SERIAL
            // Check for errors in RS485
            if (rs485_action) {
                if (!rs485_error) {

                    // There was no error, count one error less
                    if (transmission_config.rs485_errors) {
                        transmission_config.rs485_errors--;
                    }

                } else {
                    // There was an error, count one error more
                    transmission_config.rs485_errors++;
                }
            }
#endif

#if DEBUG_TRANSMISSION
            // print_debug("TR", stdout, CPURPLE, 0, "Actual MODEM errors: %d", transmission_config.modem_errors);
            // print_debug("TR", stdout, CPURPLE, 0, "Actual RS485 errors: %d", transmission_config.rs485_errors);
#endif

#ifdef MODEM_SERIAL
            // Check if we should RESET the MODEM (too many errors)
            if (transmission_config.modem_errors>=RESET_MODEM_AFTER_ERRORS) {
                transmission_config.modem_ready = 0;
            }
#endif

#ifdef RS485_SERIAL
            // Check if we should RESET the RS485 (too many errors)
            if (transmission_config.rs485_errors>=RESET_RS485_AFTER_ERRORS) {
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

    // Free memory
    if (buf) {
        free(buf);
    }
}
