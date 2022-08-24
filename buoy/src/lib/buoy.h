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
};
typedef struct TBuoyAcel BuoyAcel;

struct TUserRequest {
    float x;          // X target
    float y;          // Y target
    float z;          // Z target
    float dir;      // Grades 90º is North
    float acel;     // Between -5 and 5
    long int lastrequest;
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
