#ifndef BUOY_H
#define BUOY_H

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

struct TUserRequest {
    signed int x;                   // X-axis [-1000, +1000]
    signed int y;                   // Y-axis [-1000, +1000]
    signed int z;                   // Z-axis [-1000, +1000]
    signed int r;                   // R-axis [-1000, +1000]
    unsigned int buttons1;          // Buttons 1 :: Bits [but15, but14, ..., but1]
    unsigned int buttons2;          // Buttons 2 :: Bits [but31, but30, ..., but16]
    unsigned short int extension;   // Extensions enabled:   0:pitch    1:roll
    signed int pitch;               // Pitch [-1000, +1000]
    signed int roll;                // Roll  [-1000, +1000]
};
typedef struct TUserRequest UserRequest;

struct TWaterAnalisys {
    float ph;               // PH
    float ph_temp;          // PH Temperature
    float orp;              // ORP
    float orp_temp;         // ORP Temperature
    float conductivity;     // Conductivity
    float turbidity;        // Turbidity
};
typedef struct TWaterAnalisys WaterAnalisys;

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
