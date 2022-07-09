#ifndef CONSTANTS_H
#define CONSTANTS_H

// System Identification
#define SYSNAME "Buoy"
#define VERSION "1.0"

#define MANAGER_HOST "manager.salmonete.com"
#define MANAGER_PORT 80
#define MANAGER_URI "/register.php"

#define EXTERNAL_PORT 3814
#define EXTERNAL_PASSWORD "thisismypassword"

#define RESET_MODEM_AFTER_ERRORS 10
#define RESET_RS485_AFTER_ERRORS 10
#define TIMEOUT_MODEM_MS 500
#define TIMEOUT_RS485_MS 500

#define DEFAULT_SERIAL_WAIT 500
#define DEFAULT_SERIAL_STEP 10

#define SERIAL_RXD1 9
#define SERIAL_TXD1 10
#define SERIAL_RXD2 16
#define SERIAL_TXD2 17

#endif
