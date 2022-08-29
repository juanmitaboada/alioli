#ifndef CONFIG_H
#define CONFIG_H

#include "key.h"

// General config
#define QUICK_START 0
#define RTC 1
#define VERIFY_REMOTE_SERVER_ANSWER 1
#define DUMMY_GPS 1

// Debugger
#define DEBUG
#define DEBUG_MAIN 0
#define DEBUG_MODEM_SETUP 1
#define DEBUG_TRANSMISSION 1
#define DEBUG_TRANSMISSION_MSG 1
#define DEBUG_EXPECTED 1

// Lookups
#define TEMPERATURE_LOOKUP_MS 500

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

// GYROSCOPE
#define GYROSCOPE_MPU_MUST_EXIST 1  // MPU device must exists
#define GYROSCOPE_MPU 0x69          // I2C address of the MPU-6050 (68h, addre=1:69h)
// #define GYROSCOPE_BNO_MUST_EXIST 0  // BNO device must exists
// #define GYROSCOPE_BNO 0x28          // I2C address of the BNO-055 (28h, addr=1:29h)

// Temperature
// #define PINOUT_ONEWIRE_BUS_pin 3    // Digital pin
// #define TEMPERATURE_SENSOR_1 {0x28, 0x6A, 0x30, 0x45, 0x92,  0x5, 0x2, 0xD3}
// #define TEMPERATURE_SENSOR_2 {0x28, 0x2E, 0xEC, 0x45, 0x92, 0x15, 0x2, 0xE9}

// Transmission
// Valid speed (baud rates): 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, and 115200
#define SERIAL1_SPEED 115200        // Serial 1 port speed or baud rates
#define SERIAL2_SPEED 115200        // Serial 2 port speed or baud rates
#define SERIAL2_TX_PIN 8           // Serial 2 pin for control the status of the module
#define SERIAL2_RX_PIN 9           // Serial 2 pin for control the status of the module
#define SERIAL2_CTRLPIN 3           // Serial 2 pin for control the status of the module
#define TRANSMISSION_MS 500
#define TRANSMISSION_GPS_MS 10000
#define TRANSMISSION_OURIP_MS 30000
#define TRANSMISSION_WEBSERVER_MS 30000
#define MODEM_SERIAL 1
#define RS485_SERIAL 2
#define MODEM_PIN 2002
#define MODEM_SIMULATED 0

#endif
