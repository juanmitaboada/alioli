#ifndef SERIAL_H
#define SERIAL_H

#include "alioli.h"

#define SERIAL_MAX_BYTES 2048

size_t serial_write(int serial, const unsigned char *msg, size_t bytes);
unsigned short int serial_flush(int serial);
unsigned short int serial_available(int serial);
int serial_read(int serial);
unsigned short int serial_send(int serial, const char *msg, size_t bytes);
unsigned short int serial_recv(int serial, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned long int wait, unsigned long int wait_transfer, unsigned short int newlines);
unsigned short int serial_cmd(int serial, const char *header, const char *cmd, const char *expected, char **buf, size_t *buf_size, size_t *buf_allocated, unsigned long int wait, unsigned long int wait_transfer, unsigned short int newlines);
void serial_setup();

#endif
