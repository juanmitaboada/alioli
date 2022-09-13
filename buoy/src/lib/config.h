#ifndef CONFIG_H
#define CONFIG_H

#include "key.h"

// General config
#define QUICK_START 0
#define RTC 1
#define VERIFY_REMOTE_SERVER_ANSWER 1
#define DUMMY_GPS 1
#define OPTIMIZE 0

// Debugger
#define DEBUG
#define DEBUG_MAIN 0
#define DEBUG_MODEM_SETUP 0
#define DEBUG_TRANSMISSION 0
#define DEBUG_TRANSMISSION_MSG 0
#define DEBUG_EXPECTED 0
#define DEBUG_SENSORS 0
#define DEBUG_SENSORS_TEMPERATURE 0
#define DEBUG_SENSORS_POWER 0

// Lookups
#define TEMPERATURE_LOOKUP_MS 500
#define POWER_LOOKUP_MS 500

// Lights
#define LIGHTS 0
#define ONBOARD_LIGHT_PINOUT 25     // Onboard led
#define ONBOARD_LIGHT_ON_MS 100
#define ONBOARD_LIGHT_OFF_MS 900
#define RED_LIGHT_PINOUT 34         // Digital pin to Relay IN 1
#define RED_LIGHT_ON_MS 100
#define RED_LIGHT_OFF_MS 1900
#define GREEN_LIGHT_PINOUT 35       // Digital pin to Relay IN 2
#define GREEN_LIGHT_ON_MS 100
#define GREEN_LIGHT_OFF_MS 200

// I2C Bus
#define BUS_I2C_SDA 8
#define BUS_I2C_SCL 9

// POWER SENSOR
#define POWER_INA219_MUST_EXIST 1   // INA219 device must exists

// Temperature
#define PINOUT_ONEWIRE_BUS_PIN 15   // Digital pin (GP15)
#define TEMPERATURE_SENSOR_1 {0x28, 0xFF, 0x64, 0x1E, 0x5F, 0xBD, 0x68, 0x63}
#define TEMPERATURE_SENSOR_2 {0x28, 0xFF, 0x64, 0x1E, 0x5F, 0xBB, 0x7D, 0x6B}

// Transmission
// Valid speed (baud rates): 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, and 115200
#define SERIAL1_MAX_BYTES 512       // Serial 1 max bytes to read at once
#define SERIAL1_SPEED 115200        // Serial 1 port speed or baud rates
#define SERIAL2_SPEED 115200        // Serial 2 port speed or baud rates
#define SERIAL2_TX_PIN 8            // Serial 2 pin for control the status of the module
#define SERIAL2_RX_PIN 9            // Serial 2 pin for control the status of the module
#define SERIAL2_CTRLPIN 3           // Serial 2 pin for control the status of the module
#define TRANSMISSION_MS 1
#define TRANSMISSION_GPS_MS 10000
#define TRANSMISSION_OURIP_MS 50000
#define TRANSMISSION_WEBSERVER_MS 60000
#define TRANSMISSION_PING_ROV_MS 10000
#define MODEM_SERIAL 1
#define RS485_SERIAL 2
#define MODEM_PIN 2002
#define MODEM_SIMULATED 0

#endif
