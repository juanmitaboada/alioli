#include "serial.h"
#include "alioli.h"

static char SERIAL_NAME[] ="SERIAL";

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
        print_debug(SERIAL_NAME, stderr, CRED, 0, "ERROR: serial_write() has failed because serial port number %d is not declared", serial);
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
        print_debug(SERIAL_NAME, stderr, CRED, 0, "ERROR: serial_flush() has failed because serial port number %d is not declared", serial);
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
        print_debug(SERIAL_NAME, stderr, CRED, 0, "ERROR: serial_available() has failed because serial port number %d is not declared", serial);
        avail = 0;
    }
    return avail;
}

int serial_read(int serial) {
    int incomingByte=-1;
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
        print_debug(SERIAL_NAME, stderr, CRED, 0, "ERROR: serial_read() has failed because serial port number %d is not declared", serial);
        incomingByte = -1;
    }
    return incomingByte;
}

unsigned short int serial_send(int serial, const char *msg, size_t bytes) {
    size_t just_sent=0, sent=0;
    unsigned short int error=0;
    short int ctrlpin=-1;
    long int bauds=0, waitbuffer=0;

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
        print_debug(SERIAL_NAME, stderr, CRED, 0, "ERROR: serial_send() has failed because serial port number %d is not declared", serial);
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

unsigned short int serial_recv(int serial, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait, unsigned int wait_transfer, unsigned short int newlines) {
    unsigned short int error=0, newline_counted=0;
    int incomingByte = 0;
    unsigned int start=0;
    long int bauds=0, waitbuffer=0;
    size_t max_bytes = SERIAL_MAX_BYTES;

    if (0) {
#ifdef SERIAL1_SPEED
    } else if (serial==1) {
#ifdef SERIAL1_CTRLPIN
        bauds = SERIAL1_SPEED;
#endif
#ifdef SERIAL1_MAX_BYTES
        max_bytes = SERIAL1_MAX_BYTES;
#endif
#endif
#ifdef SERIAL2_SPEED
    } else if (serial==2) {
#ifdef SERIAL2_CTRLPIN
        bauds = SERIAL2_SPEED;
#endif
#ifdef SERIAL2_MAX_BYTES
        max_bytes = SERIAL2_MAX_BYTES;
#endif
#endif
#ifdef SERIAL3_SPEED
    } else if (serial==3) {
#ifdef SERIAL3_CTRLPIN
        bauds = SERIAL3_SPEED;
#endif
#ifdef SERIAL3_MAX_BYTES
        max_bytes = SERIAL3_MAX_BYTES;
#endif
#endif
    } else {
        print_debug(SERIAL_NAME, stderr, CRED, 0, "ERROR: serial_recv() has failed because serial port number %d is not declared", serial);
        error = 1;
    }

    // Give time to answer
    if (!serial_available(serial)) {
        start = millis();
        while (millis()-start<wait) {
            if (serial_available(serial)) {
                break;
            } else {
                yield();
            }
        }
    }

    // If forced delay, just wait for transfer of data to happen
    if (wait_transfer) {
        start = millis();
        while (millis()-start<wait_transfer) {
            yield();
        }
    }

    // While data in the bus and not received MAX data transfer allowed per cycle, process it
    while (serial_available(serial) && (*buf_size<max_bytes)) {
        incomingByte = serial_read(serial);
        if (incomingByte>=0) {

            // Count new lines if requested and we passed the first 2 bytes
            if (newlines && (*buf_size>=2) && ((char) incomingByte == '\n')) {
                // We found '\n'
                newline_counted++;
            }

            // Concat
            if (!strcat_realloc(buf, buf_size, buf_allocated, (char*) &incomingByte, 1, __FILE__, __LINE__)) {
                error = 1;
            }

            // Decide if we should keep reading
            if (newlines && (newlines==newline_counted)) {
                // We already got the newlines we were expecting, stop reading!
                break;
            }
        } else {
            error = 1;
        }

        // Wait until another byte arrives
        // (bauds/10=bytes/sec, bauds/10/1000 = bytes/msec, bytes/speed = ms to wait + 1)
        waitbuffer = bauds/10.0;
        if (waitbuffer>=1000) {
            waitbuffer = waitbuffer / 1000;
        } else {
            waitbuffer = 1;
        }
        delay(1+waitbuffer);
        yield();
    }

    return !error;

}

unsigned short int serial_cmd(int serial, const char *header, const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned int wait, unsigned int wait_transfer, unsigned short int newlines) {
    unsigned short int error=0;
    char *check=NULL;
    unsigned int check_size=0;

    // Reset buffer size
    (*buf_size) = 0;

    // Flush buff
    serial_flush(serial);

    // Say hello to MODEM
    if (serial_send(serial, cmd, strlen(cmd))) {

        // Receive data
        if (serial_recv(serial, buf, buf_size, buf_allocated, wait, wait_transfer, newlines)) {
            if (((*buf_size)>2) && ((*buf)[0]=='\r') && ((*buf)[1]=='\n')) {
                check = (*buf)+2;
                check_size = (unsigned int) (*buf_size)-1;
            } else {
                check = *buf;
                check_size = (unsigned int) (*buf_size);
            }
            if (expected && bistrcmp(expected, strlen(expected), check, min(strlen(expected), check_size))) {
                print_debug(header, stderr, CRED, 0, "Unexpected answer for command '%s'", cmd);
#if DEBUG_EXPECTED
                print_asbin(check, check_size, stderr);
                print_ashex(check, check_size, stderr);
                print_debug(header, stderr, CRED, 0, "expected:");
                print_asbin(expected, strlen(expected), stderr);
                print_ashex(expected, strlen(expected), stderr);
#endif
                error = 1;
            }
        } else {
            print_debug(header, stderr, CRED, 0, "'%s' command ERROR!!!", cmd);
            error = 1;
        }

    } else {
        print_debug(header, stdout, CRED, 0, "Nothing sent for command '%s'!!!???", cmd);
        error = 1;
    }

    return !error;
}

void serial_setup() {
#if DEBUG_SERIAL
    print_debug(SERIAL_NAME, stdout, CPURPLE, COLOR_NOTAIL, "SETUP INI -> ");
#endif

#ifdef SERIAL1_SPEED
#if DEBUG_SERIAL
    print_debug(SERIAL_NAME, stdout, CPURPLE, COLOR_NOHEAD_NOTAIL, "SERIAL1 ");
#endif
#ifdef SERIAL1_CTRLPIN
    pinMode(SERIAL1_CTRLPIN, OUTPUT);
    digitalWrite(SERIAL1_CTRLPIN, LOW);
#endif
#ifndef ESP32
    Serial1.begin(SERIAL1_SPEED, SERIAL_8N1);
#else
    Serial1.begin(SERIAL1_SPEED, SERIAL_8N1, SERIAL_RXD1, SERIAL_TXD1);
    Serial1.setRxBufferSize(65535);
#endif
#endif
#ifdef SERIAL2_SPEED
#if DEBUG_SERIAL
    print_debug(SERIAL_NAME, stdout, CPURPLE, COLOR_NOHEAD_NOTAIL, "SERIAL2 ");
#endif
#ifdef SERIAL2_CTRLPIN
    pinMode(SERIAL2_CTRLPIN, OUTPUT);
    digitalWrite(SERIAL2_CTRLPIN, LOW);
#endif
#ifndef ESP32
    Serial2.begin(SERIAL1_SPEED, SERIAL_8N1);
#else
    Serial2.begin(SERIAL2_SPEED, SERIAL_8N1, SERIAL_RXD2, SERIAL_TXD2);
    Serial2.setRxBufferSize(65535);
#endif
#endif
#ifdef SERIAL3_SPEED
#if DEBUG_SERIAL
    print_debug(SERIAL_NAME, stdout, CPURPLE, COLOR_NOHEAD_NOTAIL, "SERIAL3 ");
#endif
#ifdef SERIAL3_CTRLPIN
    pinMode(SERIAL3_CTRLPIN, OUTPUT);
    digitalWrite(SERIAL3_CTRLPIN, LOW);
#endif
#ifndef ESP32
    Serial3.begin(SERIAL1_SPEED, SERIAL_8N1);
#else
    Serial3.begin(SERIAL3_SPEED);
    Serial3.setRxBufferSize(65535);
#endif
#endif

#if DEBUG_SERIAL
    // Show we are done
    print_debug(SERIAL_NAME, stdout, CGREEN, COLOR_NOHEAD, "SETUP DONE");
#endif
}
