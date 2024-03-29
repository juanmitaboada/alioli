#ifndef CONFIG_H
#define CONFIG_H

// General config
#define QUICK_START 0
#define ENGINE_TEST 0
#define RTC 1
#define OPTIMIZE 1

// MODULES
#define MODULE_OSD 1
#define MODULE_SENSORS 1
#define MODULE_ENGINES 1
#define MODULE_BRAIN 1
#define MODULE_CONTROL 1
#define MODULE_MONITOR 1
#define MODULE_MONITOR_RAM 0
#define MODULE_MONITOR_LIGHTS 1

// Debugger
#define ALIOLI_ASSERTS 1
#define DEBUG_NO
#define DEBUG_SETUP 0
#define DEBUG_MAIN 0
#define DEBUG_CONTROL 0
#define DEBUG_SENSORS 0
#define DEBUG_SENSORS_GYROSCOPE 0
#define DEBUG_SENSORS_PRESSURE 0
#define DEBUG_SENSORS_POWER 0
#define DEBUG_SENSORS_ANALISYS 0
#define DEBUG_SENSORS_TEMPERATURE 0
#define DEBUG_BRAIN 0
#define DEBUG_ENGINES 0
#define DEBUG_MONITOR 0
#define DEBUG_TRANSMISSION 0
#define DEBUG_TRANSMISSION_MSG 0
#define DEBUG_COMMUNICATION 0
#define DEBUG_OSD 0

// Lookups
// #define PAD_LOOKUP_MS 500
#define GYROSCOPE_LOOKUP_MS 500
#define TEMPERATURE_LOOKUP_MS 500
#define PRESSURE_LOOKUP_MS 500
#define POWER_LOOKUP_MS 500
#define BRAIN_LOOKUP_MS 500
#define ANALISYS_LOOKUP_MS 500
#define BUOYANCY_LOOKUP_MS 500
// #define OSD_LOOKUP_MS 5000
#define OSD_LOOKUP_MS 500
#define MONITOR_RAM_LOOKUP_MS 10000

// PAD
// #define PAD 0
// #define DEFAULT_CONTROL_PAD_X 500
// #define DEFAULT_CONTROL_PAD_Y 1000

// Lights
#define ONBOARD_LIGHT_PINOUT 13     // Onboard led
#define ONBOARD_LIGHT_ON_MS 100
#define ONBOARD_LIGHT_OFF_MS 900
#define RED_LIGHT_PINOUT 34         // Digital pin to Relay IN 1
#define RED_LIGHT_ON_MS 100
#define RED_LIGHT_OFF_MS 1900
#define GREEN_LIGHT_PINOUT 35       // Digital pin to Relay IN 2
#define GREEN_LIGHT_ON_MS 100
#define GREEN_LIGHT_OFF_MS 200

// POWER SENSOR
#define POWER_INA219_MAIN_MUST_EXIST 1      // INA219 MAIN device must exists (power sensor)
#define POWER_INA219_EXTERNAL_MUST_EXIST 0  // INA219 EXTERNAL device must exists (power sensor)
#define POWER_INA219_EXTERNAL_ADDR 0x41     // INA219 device address for INA219 sensor External Battery

// PAD
// #define PAD_PINOUT_X_pin 0          // Analog pin connected to X output
// #define PAD_PINOUT_Y_pin 1          // Analog pint connected to Y output
// #define PAD_PINOUT_SW_pin 2         // Digital pin connected to switch output

// GYROSCOPE
#define GYROSCOPE_MPU_MUST_EXIST 0  // MPU device must exists
#define GYROSCOPE_MPU_NO 0x69          // I2C address of the MPU-6050 (68h, addre=1:69h)
#define GYROSCOPE_BNO_MUST_EXIST 1  // BNO device must exists
#define GYROSCOPE_BNO 0x28          // I2C address of the BNO-055 (28h, addr=1:29h)

// Temperature
#define PINOUT_ONEWIRE_BUS_pin 3    // Digital pin
#define TEMPERATURE_MAIN_BATTERY {0x28, 0xFF, 0x64, 0x1E, 0x5F, 0xBB, 0x7D, 0x6B}    // Internal Bat 11.1V
#define TEMPERATURE_ENGINES_BATTERY {0x28, 0xFF, 0x64, 0x1E, 0x23, 0x89, 0xC7, 0xCF} // Engines Bat 12V
#define TEMPERATURE_SEA_WATER {0x28, 0xFF, 0x64, 0x1E, 0x5F, 0xBD, 0x68, 0x63}       // Sea Water

// Pressure
#define PRESSURE_SENSOR_ADDRESS 0x76

// Buoyancy position:
//          ^           
//      ^       ^       
//      1   ^   4       
//  ^   2   |   3   ^   
//                      
// Buoyancy pinout
#define BUOYANCY_IN_FRONT_LEFT 36
#define BUOYANCY_IN_FRONT_RIGHT 37
#define BUOYANCY_IN_BACK_LEFT 38
#define BUOYANCY_IN_BACK_RIGHT 39
#define BUOYANCY_OUT_FRONT_LEFT 40
#define BUOYANCY_OUT_FRONT_RIGHT 41
#define BUOYANCY_OUT_BACK_LEFT 42
#define BUOYANCY_OUT_BACK_RIGHT 43
// Buoyancy config
#define BUOYANCY_TOKEN_MS 500

// Engines position:
//          ^           
//      ^       ^       
//      1   ^   6       
//  ^   2   |   5   ^   
//      3       4       
//                      
// Engines pinout
#define ENGINE_FRONT_RIGHT_DIR 22
#define ENGINE_FRONT_RIGHT_ENABLE 28
#define ENGINE_LATERAL_RIGHT_DIR 23
#define ENGINE_LATERAL_RIGHT_ENABLE 29
#define ENGINE_BACK_RIGHT_DIR 24
#define ENGINE_BACK_RIGHT_ENABLE 30
#define ENGINE_BACK_LEFT_DIR 25
#define ENGINE_BACK_LEFT_ENABLE 31
#define ENGINE_LATERAL_LEFT_DIR 26
#define ENGINE_LATERAL_LEFT_ENABLE 32
#define ENGINE_FRONT_LEFT_DIR 27
#define ENGINE_FRONT_LEFT_ENABLE 33

// Analisys pinout
#define ANALISYS_PH 8
#define ANALISYS_PH_TEMP 9
#define ANALISYS_ORP 10
#define ANALISYS_ORP_TEMP 11
#define ANALISYS_CONDUCTIVITY 7
#define ANALISYS_TURBIDITY 6

// Transmission
// Valid speed (baud rates): 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600 and 115200
#define SERIAL1_PINOUT 4            // Serial 1 digital pin connected to DE
#define SERIAL1_SPEED 115200        // Serial 1 port speed or baud rates
#define SERIAL1_CTRLPIN 4           // Serial 1 pin for control the status of the module
#define SERIAL2_SPEED 57600         // Serial 2 pin for control the MiniMicroOSD module
#define TRANSMISSION_MS 0
#define RS485_SERIAL 1
#define OSD_SERIAL 2

#endif
