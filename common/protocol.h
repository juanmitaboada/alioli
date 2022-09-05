#ifndef PROTOCOL_H
#define PROTOCOL_H

// Protocol definition
// [HEADER: 2 bytes] [PAYLOAD_SIZE] [PAYLOAD] [CRC: 2 bytes]

// Protocol header
#define ALIOLI_MAGIC_HEADER 911


// === General protocol fields ===
//
typedef struct TAlioliProtocol {
    unsigned char header[2];
    unsigned int payload_size;
    char *payload;
    unsigned char crc[2];
} AlioliProtocol;

// === ROV Control ===

typedef struct TUserRequest {
    signed int x;                   // X-axis [-1000, +1000]
    signed int y;                   // Y-axis [-1000, +1000]
    signed int z;                   // Z-axis [-1000, +1000]
    signed int r;                   // R-axis [-1000, +1000]
    unsigned int buttons1;          // Buttons 1 :: Bits [but15, but14, ..., but1]
    unsigned int buttons2;          // Buttons 2 :: Bits [but31, but30, ..., but16]
    unsigned short int extension;   // Extensions enabled:   0:pitch    1:roll
    signed int pitch;               // Pitch [-1000, +1000]
    signed int roll;                // Roll  [-1000, +1000]
} UserRequest;


// === ROV Status ===

typedef struct TAcelerometer {
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
} Acelerometer;

typedef struct TWaterAnalisys {
    float ph;               // PH
    float ph_temp;          // PH Temperature
    float orp;              // ORP
    float orp_temp;         // ORP Temperature
    float conductivity;     // Conductivity
    float turbidity;        // Turbidity
} WaterAnalisys;

typedef struct TROVStatus {
    float altitude;
    float pressure;
    float temperaturegy;
    float temperature1;     // Sensor
    float temperature2;     // Sensor
    float temperaturebmp;   // BMP Sensor
    float voltage;          // INA Sensor (Power sensor)
    float amperage;         // INA Sensor (Power sensor)
    Acelerometer acelerometer;
    WaterAnalisys analisys;

    // Extra controls
    unsigned short int error_code;
} ROVStatus;


#endif
