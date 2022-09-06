#ifndef BUOY_H
#define BUOY_H

#include "common/protocol.h"

// === BUOY ACELEROMETER ===
struct TBuoyGPS {
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float course;
    unsigned long int epoch;
    unsigned short int newdata;
};
typedef struct TBuoyGPS BuoyGPS;

struct TBuoyAcel {
    // int AcX;
    // int AcY;
    // int AcZ;
    int Tmp;
    // int GyX;
    // int GyY;
    // int GyZ;
    float angx;
    float angy;
    float angz;
    unsigned short int newdata;
};
typedef struct TBuoyAcel BuoyAcel;

struct TEnvironment {
    float altitude;
    float pressure;
    float temperaturegy;
    float temperature1;     // Sensor
    float temperature2;     // Sensor
    float temperaturebmp;   // BMP Sensor
    float voltage;          // INA Sensor (Power sensor)
    float amperage;         // INA Sensor (Power sensor)
    WaterAnalisys analisys;
};
typedef struct TEnvironment Environment;

struct TBuoy {
    BuoyGPS gps;
    BuoyAcel acel;
    Environment environment;
    UserRequest user;
};
typedef struct TBuoy Buoy;

#endif
