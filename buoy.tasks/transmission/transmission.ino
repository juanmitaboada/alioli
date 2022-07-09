// #include "lib/shared.h"
// #include "lib/config.h"
#include "lib/version.h"
#include "lib/alioli.h"

#include "transmission.h"
#include "communication.h"

TransmissionConfig transmission_config;

size_t serial_write(int serial, const unsigned char *msg, size_t bytes) {
    size_t sent=0;
    if (0) {
#ifdef SERIAL1_SPEED
    } else if (serial==1) {
        sent = Serial1.write(msg, bytes);
#endif
#ifdef SERIAL2_SPEED
    } else if (serial==2) {
        sent = Serial2.write(msg, bytes);
#endif
#ifdef SERIAL3_SPEED
    } else if (serial==3) {
        sent = Serial3.write(msg, bytes);
#endif
    } else {
        print_debug("TR", stderr, CRED, 0, "ERROR: serial_write() has failed because serial port number %d is not declared", serial);
        sent = -1;
    }
    return sent;
}
unsigned short int serial_flush(int serial) {
    unsigned short int done=1;
    if (0) {
#ifdef SERIAL1_SPEED
    } else if (serial==1) {
        Serial1.flush();
#endif
#ifdef SERIAL2_SPEED
    } else if (serial==2) {
        Serial2.flush();
#endif
#ifdef SERIAL3_SPEED
    } else if (serial==3) {
        Serial3.flush();
#endif
    } else {
        print_debug("TR", stderr, CRED, 0, "ERROR: serial_flush() has failed because serial port number %d is not declared", serial);
        done=0;
    }
    return done;
}

unsigned short int serial_available(int serial) {
    unsigned short int avail=0;
    if (0) {
#ifdef SERIAL1_SPEED
    } else if (serial==1) {
        avail = Serial1.available()>0;
#endif
#ifdef SERIAL2_SPEED
    } else if (serial==2) {
        avail = Serial2.available()>0;
#endif
#ifdef SERIAL3_SPEED
    } else if (serial==3) {
        avail = Serial3.available()>0;
#endif
    } else {
        print_debug("TR", stderr, CRED, 0, "ERROR: serial_available() has failed because serial port number %d is not declared", serial);
        avail = 0;
    }
    return avail;
}

unsigned short int serial_read(int serial) {
    unsigned short int incomingByte=-1;
    if (0) {
#ifdef SERIAL1_SPEED
    } else if (serial==1) {
        incomingByte = Serial1.read();
#endif
#ifdef SERIAL2_SPEED
    } else if (serial==2) {
        incomingByte = Serial2.read();
#endif
#ifdef SERIAL3_SPEED
    } else if (serial==3) {
        incomingByte = Serial3.read();
#endif
    } else {
        print_debug("TR", stderr, CRED, 0, "ERROR: serial_read() has failed because serial port number %d is not declared", serial);
        incomingByte = -1;
    }
    return incomingByte;
}

unsigned short int serial_send(int serial, const char *msg, size_t bytes) {
    size_t just_sent=0, sent=0;
    unsigned short int error=0;
    short int ctrlpin=-1;
    int bauds=0, waitbuffer=0;

    if (0) {
#ifdef SERIAL1_SPEED
    } else if (serial==1) {
#ifdef SERIAL1_CTRLPIN
        ctrlpin = SERIAL1_CTRLPIN;
        bauds = SERIAL1_SPEED;
#endif
#endif
#ifdef SERIAL2_SPEED
    } else if (serial==2) {
#ifdef SERIAL2_CTRLPIN
        ctrlpin = SERIAL2_CTRLPIN;
        bauds = SERIAL2_SPEED;
#endif
#endif
#ifdef SERIAL3_SPEED
    } else if (serial==3) {
#ifdef SERIAL3_CTRLPIN
        ctrlpin = SERIAL3_CTRLPIN;
        bauds = SERIAL3_SPEED;
#endif
#endif
    } else {
        print_debug("TR", stderr, CRED, 0, "ERROR: serial_send() has failed because serial port number %d is not declared", serial);
        error = 1;
    }

    // If no error until here
    if (!error) {

        // Prepare for sending
        if (ctrlpin>=0) {
            digitalWrite(ctrlpin, HIGH);
        }

        // Make sure all data is sent
        while (sent<bytes) {
            yield();

            // Send
            just_sent = serial_write(serial, (const unsigned char*) (msg+sent), bytes-sent);

            // Check if we sent something
            if (just_sent) {

                // Remember how much has been sent
                sent += just_sent;

                // Flush the buffer
                serial_flush(serial);

            } else {
                // Nothing sent, return with error
                error = 1;
                break;
            }
        }

        // Wait until all bytes have been sent
        // (bauds/10=bytes/sec, bauds/10/1000 = bytes/msec, bytes/speed = ms to wait + 1)
        waitbuffer = bauds/10.0;
        if (waitbuffer>=1000) {
            waitbuffer = waitbuffer / 1000;
        } else {
            waitbuffer = 1;
        }
        delay(1+waitbuffer);

        // Finish sending
        if (ctrlpin>=0) {
            digitalWrite(ctrlpin, LOW);
        }

    }

    // Return
    return !error;
}

unsigned short int serial_recv(int serial, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    unsigned short int error = 0;
    unsigned short int incomingByte = 0;
    unsigned int start=0;

    // Give time to answer
    if (wait && (!serial_available(serial))) {
        start = millis();
        while (((unsigned int)millis())-start<wait) {
            if (serial_available(serial)) {
                break;
            }
        }
    }

    // While data in the bus, process it
    while (serial_available(serial)) {
        incomingByte = serial_read(serial);
        if (incomingByte>=0) {
            if (!strcat_realloc(buf, buf_size, buf_allocated, (char*) &incomingByte, 1, __FILE__, __LINE__)) {
                error = 1;
            }
        } else {
            error = 1;
        }
    }

    return !error;

}

void serial_sendmsg(Bus *bus, const char *buf, size_t buf_size) {
    xSemaphoreTake(bus->out.lock, portMAX_DELAY);
    strcat_realloc(&(bus->out.buf), &(bus->out.size), &(bus->out.allocated), buf, buf_size, __FILE__, __LINE__);
    xSemaphoreGive(bus->out.lock);
}

size_t serial_getmsg(Bus *bus, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    unsigned int start=0;

    // Initialize
    (*buf_size) = 0;

    // Give time to answer
    if (wait && (!bus->in.size)) {
        start = millis();
        while (((unsigned int)millis())-start<wait) {
            if (bus->in.size) {
                break;
            }
        }
    }

    // Read buffer
    if (bus->in.size) {
        xSemaphoreTake(bus->in.lock, portMAX_DELAY);
        strcat_realloc(buf, buf_size, buf_allocated, bus->in.buf, bus->in.size, __FILE__, __LINE__);
        bus->in.size=0;
        xSemaphoreGive(bus->in.lock);
    }

    return *buf_size;
}

unsigned short int serial_cmd(Bus *bus, const char *header, const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    unsigned short int error=0;
    unsigned int i=0, start=0;

    // Reset buffer size
    (*buf_size) = 0;

    // Say hello to MODEM
    serial_sendmsg(bus, cmd, strlen(cmd));

    // Receive data
    if (serial_getmsg(bus, buf, buf_size, buf_allocated, wait)) {
        if (expected && bistrcmp(expected, strlen(expected), *buf, min(strlen(expected), (unsigned int) *buf_size))) {
            print_debug(header, stderr, CRED, 0, "Unexpected answer for command '%s'", cmd);
#if DEBUG_EXPECTED
            print_asbin(*buf, *buf_size);
            print_ashex(*buf, *buf_size);
            print_debug(header, stderr, CRED, 0, "expected:");
            print_asbin(expected, strlen(expected));
            print_ashex(expected, strlen(expected));
#endif
            error = 1;
        }
    } else if (expected) {
        print_debug(header, stderr, CRED, 0, "'%s' command ERROR!!!", cmd);
        error = 1;
    }

    return !error;
}

#ifdef MODEM_SERIAL
unsigned short int modem_cmd(const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    return serial_cmd(&(transmission_config.modem), "TR-M", cmd, expected, buf, buf_size, buf_allocated, wait);
}

size_t modem_getmsg(char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    return serial_getmsg(&(transmission_config.modem), buf, buf_size, buf_allocated, wait);
}
void modem_sendmsg(char *answer, size_t answer_size) {
    serial_sendmsg(&(transmission_config.modem), answer, answer_size);
}

unsigned short int modem_gps(char **buf, size_t *buf_size, size_t *buf_allocated) {
    unsigned short int error=0;

    // Initialize
    (*buf_size) = 0;

    return !error;
}
#endif

#ifdef RS485_SERIAL
size_t rs485_getmsg(char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    return serial_getmsg(&(transmission_config.rs485), buf, buf_size, buf_allocated, wait);
}
void rs485_sendmsg(char *buf, size_t buf_size) {
    serial_sendmsg(&(transmission_config.rs485), buf, buf_size);
}
unsigned short int rs485_cmd(const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait) {
    return serial_cmd(&(transmission_config.rs485), "TR-R", cmd, expected, buf, buf_size, buf_allocated, wait);
}
#endif

void serial_task(Bus *bus) {
    int gotmsg=0;

    // Send if anything to be sent
    if (bus->out.size) {
        xSemaphoreTake(bus->out.lock, portMAX_DELAY);
        serial_send(bus->port, bus->out.buf, bus->out.size);
        bus->out.size=0;
        xSemaphoreGive(bus->out.lock);
    }

    // Check if something comming
    xSemaphoreTake(bus->in.lock, portMAX_DELAY);
    gotmsg = serial_recv(bus->port, &(bus->in.buf), &(bus->in.size), &(bus->in.allocated), 0);
    xSemaphoreGive(bus->in.lock);
    yield();
    delay(10);
}
void modem_serial_task(void *arg) {
    for (;;) {
        serial_task(&(transmission_config.modem));
    }
}
void rs485_serial_task(void *arg) {
    for (;;) {
        serial_task(&(transmission_config.rs485));
    }
}

#ifdef MODEM_SERIAL
unsigned short int modem_setup() {
    char *buf=NULL, *cmd=NULL, *pointer=NULL;
    size_t buf_size=0, buf_allocated=0;
    const char *msg=NULL;
    unsigned short int error=0, pin_ready=0, i=0, j=0, stat=0, retry=0;

    // Prepare cmd
    cmd = (char*) malloc(sizeof(char)*1024);
    cmd[0]=0;

    // STEP 0
    if (transmission_config.modem_step==0) {
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

    // STEP 1
    } else if (transmission_config.modem_step==1) {
        // Remove ECHO
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "ECHO OFF");
#endif
            error = !modem_cmd("ATE0\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000);
        }

    // STEP 2
    } else if (transmission_config.modem_step==2) {

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

    // STEP 3
    } else if (transmission_config.modem_step==3) {

#if MODEM_SIMULATED==0

        // RESET Module
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "Reset MODULE (10 sec)");
#endif
            error = !modem_cmd("AT+CFUN=6\r\n", "\r\nOK", &buf, &buf_size, &buf_allocated, 10000);
        }

    // STEP 4
    } else if (transmission_config.modem_step==4) {

        // Startup Module
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "Enable MODULE (15 sec)");
#endif
            error = !modem_cmd("AT+CFUN=1\r\n", NULL, &buf, &buf_size, &buf_allocated, 15000);
        }

    // STEP 5
    } else if (transmission_config.modem_step==5) {

        // Reset modem
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "RESET");
#endif
            error = !modem_cmd("ATZ\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
            delay(2000);
        }

    // STEP 6
    } else if (transmission_config.modem_step==6) {

 
        // Remove ECHO
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "ECHO OFF");
#endif
            error = !modem_cmd("ATE0\r\n", NULL, &buf, &buf_size, &buf_allocated, 1000);
        }

    // STEP 7
    } else if (transmission_config.modem_step==7) {

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

    // STEP 8
    } else if (transmission_config.modem_step==8) {

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

    // STEP 9
    } else if (transmission_config.modem_step==9) {

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

    // STEP 10
    } else if (transmission_config.modem_step==10) {

        // Set CIPMODE
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "CIPMODE=1");
#endif
            modem_cmd("AT+CIPMODE=1\r\n", NULL, &buf, &buf_size, &buf_allocated, 500);
        }

    // STEP 11
    } else if (transmission_config.modem_step==11) {

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

    // STEP 12
    } else if (transmission_config.modem_step==12) {

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

    // STEP 13
    } else if (transmission_config.modem_step==13) {

        // Send our IPADDR
        if (!error) {
#if DEBUG_MODEM_SETUP
            print_debug("TRsm", stdout, CBLUE, 0, "Send our IPADDR");
#endif
            sprintf(cmd, "AT+CHTTPACT=\"%s\",%d\r\n", MANAGER_HOST, MANAGER_PORT);
            error = !modem_cmd(cmd, "\r\n+CHTTPACT: REQUEST", &buf, &buf_size, &buf_allocated, 2000);

            if (!error) {
                sprintf(cmd, "GET http://%s%s?name=%s&version=%s&build=%s&ip=%s&port=%d HTTP/1.1\r\nHost: %s\r\nUser-Agent: Alioli Buoy v%s\r\nAccept: text/html\r\nConnection: close\r\n\r\n", MANAGER_HOST, MANAGER_URI, SYSNAME, VERSION, BUILD_VERSION, transmission_config.ipaddr, EXTERNAL_PORT, MANAGER_HOST, VERSION);
                error = !modem_cmd(cmd, "\r\nOK", &buf, &buf_size, &buf_allocated, 5000);
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
                            print_debug("TRsm", stderr, CRED, 0, "Socket didn't finish transmission");
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

    // STEP 14
    } else if (transmission_config.modem_step==14) {

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

    }

    // Free memory
    free(cmd);
    if (buf) {
        free(buf);
    }

    transmission_config.modem_step++;

    return !error;
}
#endif

#ifdef RS485_SERIAL
unsigned short int rs485_setup() {
    unsigned short int error=0;
    char *buf=NULL;
    size_t buf_size=0, buf_allocated=0;

    char temp[40]="";

    transmission_config.rs485_ready = 1;
    transmission_config.rs485_errors = 0;

/*
    // Connect link to ROV
    sprintf(temp, "%d - CONNECT %d\r\n", millis(), SERIAL2_SPEED);
    // sprintf(temp, "%d - CONNECT\r\n", millis());
    if (rs485_cmd(temp, "OK", &buf, &buf_size, &buf_allocated, 2000)) {
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
*/

    return !error;
}
#endif

void transmission_setup(long int now) {
#if DEBUG_TRANSMISSION
    print_debug("TRs", stdout, CPURPLE, COLOR_NOTAIL, "INI -> ");
#endif

    // Set local config
    transmission_config.nextevent = 0;
    transmission_config.gps_nextevent = 0;
    transmission_config.modem_step = 0;
    transmission_config.modem_ready = 0;
    transmission_config.modem_errors = 0;
    transmission_config.modem_linked = 0;
    transmission_config.rs485_ready = 0;
    transmission_config.rs485_errors = 0;
    transmission_config.modem.port = MODEM_SERIAL;
    transmission_config.modem.in.buf = NULL;
    transmission_config.modem.in.size = 0;
    transmission_config.modem.in.allocated = 0;
    transmission_config.modem.in.lock = xSemaphoreCreateMutex();
    transmission_config.modem.out.buf = NULL;
    transmission_config.modem.out.size = 0;
    transmission_config.modem.out.allocated = 0;
    transmission_config.modem.out.lock = xSemaphoreCreateMutex();
    transmission_config.rs485.port = RS485_SERIAL;
    transmission_config.rs485.in.buf = NULL;
    transmission_config.rs485.in.size = 0;
    transmission_config.rs485.in.allocated = 0;
    transmission_config.rs485.in.lock = xSemaphoreCreateMutex();
    transmission_config.rs485.out.buf = NULL;
    transmission_config.rs485.out.size = 0;
    transmission_config.rs485.out.allocated = 0;
    transmission_config.rs485.out.lock = xSemaphoreCreateMutex();
    transmission_config.mavlink_buf = NULL;
    transmission_config.mavlink_buf_size = 0;
    transmission_config.mavlink_buf_allocated = 0;

    // Set up transmission port 1 - Serial 1
#ifdef SERIAL1_SPEED
#if DEBUG_TRANSMISSION
    print_debug("TRs", stdout, CPURPLE, COLOR_NOHEAD_NOTAIL, "SERIAL1 ");
#endif
#ifdef SERIAL1_CTRLPIN
    pinMode(SERIAL1_CTRLPIN, OUTPUT);
    digitalWrite(SERIAL1_CTRLPIN, LOW);
#endif
    // Serial1.begin(SERIAL1_SPEED);
    Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, SERIAL_RXD1, SERIAL_TXD1);
    Serial1.setRxBufferSize(65535);
#endif
#ifdef SERIAL2_SPEED
#if DEBUG_TRANSMISSION
    print_debug("TRs", stdout, CPURPLE, COLOR_NOHEAD_NOTAIL, "SERIAL2 ");
#endif
#ifdef SERIAL2_CTRLPIN
    pinMode(SERIAL2_CTRLPIN, OUTPUT);
    digitalWrite(SERIAL2_CTRLPIN, LOW);
#endif
    Serial2.begin(SERIAL2_SPEED, SERIAL_8N1, SERIAL_RXD2, SERIAL_TXD2);
    Serial2.setRxBufferSize(65535);
#endif
#ifdef SERIAL3_SPEED
#if DEBUG_TRANSMISSION
    print_debug("TRs", stdout, CPURPLE, COLOR_NOHEAD_NOTAIL, "SERIAL3 ");
#endif
#ifdef SERIAL3_CTRLPIN
    pinMode(SERIAL3_CTRLPIN, OUTPUT);
    digitalWrite(SERIAL3_CTRLPIN, LOW);
#endif
    Serial3.begin(SERIAL3_SPEED);
    Serial3.setRxBufferSize(65535);
#endif

    // Setup devices
#ifndef RS485_SERIAL
    transmission_config.rs485_ready = 1;
#endif
#ifndef MODEM_SERIAL
    transmission_config.modem_ready = 1;
#endif

    // Create serial port tasks
    xTaskCreate(
        modem_serial_task, /* Task function. */
        "Modem Serial Task", /* name of task. */
        10000, /* Stack size of task */
        NULL, /* parameter of the task */
        1, /* priority of the task */
        NULL); /* Task handle to keep track of created task */
    xTaskCreate(
        rs485_serial_task, /* Task function. */
        "RS485 Serial Task", /* name of task. */
        10000, /* Stack size of task */
        NULL, /* parameter of the task */
        1, /* priority of the task */
        NULL); /* Task handle to keep track of created task */

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
                    transmission_config.mavlink_buf_size = 0;

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
                    transmission_config.mavlink_buf_size = 0;

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
                    transmission_config.mavlink_buf_size = 0;

                // MAVLINK msg
                } else if (transmission_config.modem_linked) {
                    // Process message locally if linked to external system
#if DEBUG_TRANSMISSION_MSG
                    print_debug("TRl", stdout, CCYAN, 0, "LINK MAVLINK");
#endif
                    remote_msg(&buf, &buf_size, &buf_allocated, &answer, &answer_size, &answer_allocated);
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
                rs485_sendmsg(buf, buf_size);

                // Get answer
                buf_size = 0;
                if (rs485_getmsg(&buf, &buf_size, &buf_allocated, TIMEOUT_RS485_MS)) {

                    // Check if there is a message to process
                    if (buf_size) {

                        // Analize answer from rov
                        rov_msg(buf, buf_size, &answer, &answer_size, &answer_allocated);

                        // RS485 data found
                        print_debug("TRl", stdout, CPURPLE, 0, "RS485:");
                        print_asbin(answer, answer_size);
                        print_ashex(answer, answer_size);

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
                modem_sendmsg(answer, answer_size);

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
        buf = NULL;
        buf_size = 0;
    }

}
