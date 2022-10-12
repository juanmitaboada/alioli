#ifndef CONFIG_H
#define CONFIG_H

// General config
#define QUICK_START 1
#define RTC 1
#define OPTIMIZE 1

// Debugger
#define ALIOLI_ASSERTS 1
#define DEBUG_NO
#define DEBUG_MAIN 0
#define DEBUG_CONTROL 0
#define DEBUG_SENSORS 0
#define DEBUG_SENSORS_GYROSCOPE 0
#define DEBUG_SENSORS_PRESSURE 0
#define DEBUG_SENSORS_POWER 0
#define DEBUG_SENSORS_ANALISYS 0
#define DEBUG_BRAIN 0
#define DEBUG_ENGINES 0
#define DEBUG_MONITOR 0
#define DEBUG_TRANSMISSION 0
#define DEBUG_TRANSMISSION_MSG 0
#define DEBUG_COMMUNICATION 0
#define DEBUG_OSD 1

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

// PAD
// #define PAD 0
// #define DEFAULT_CONTROL_PAD_X 500
// #define DEFAULT_CONTROL_PAD_Y 1000

// Lights
#define LIGHTS 0
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
#define POWER_INA219_MUST_EXIST 0   // INA219 device must exists (power sensor)

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
#define TEMPERATURE_SENSOR_1 {0x28, 0x6A, 0x30, 0x45, 0x92,  0x5, 0x2, 0xD3}
#define TEMPERATURE_SENSOR_2 {0x28, 0x2E, 0xEC, 0x45, 0x92, 0x15, 0x2, 0xE9}

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
#define ENGINE_FRONT_LEFT_DIR 22
#define ENGINE_FRONT_LEFT_ENABLE 28
#define ENGINE_LATERAL_LEFT_DIR 23
#define ENGINE_LATERAL_LEFT_ENABLE 29
#define ENGINE_BACK_LEFT_DIR 24
#define ENGINE_BACK_LEFT_ENABLE 30
#define ENGINE_BACK_RIGHT_DIR 25
#define ENGINE_BACK_RIGHT_ENABLE 31
#define ENGINE_LATERAL_RIGHT_DIR 26
#define ENGINE_LATERAL_RIGHT_ENABLE 32
#define ENGINE_FRONT_RIGHT_DIR 27
#define ENGINE_FRONT_RIGHT_ENABLE 33

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
