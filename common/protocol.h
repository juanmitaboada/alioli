#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "alioli.h"

// Protocol definition
// [HEADER: 2 bytes] [PAYLOAD_SIZE] [PAYLOAD] [CRC: 2 bytes]

// Protocol header
#define ALIOLI_PROTOCOL_MAGIC_HEADER 911

// Package kind
#define ALIOLI_PROTOCOL_KIND_HEARTBEAT 1
#define ALIOLI_PROTOCOL_KIND_ENVIRONMENT 2
#define ALIOLI_PROTOCOL_KIND_USERREQUEST 3

// Protocol reading status
#define ALIOLI_PROTOCOL_READING_NODATA 0
#define ALIOLI_PROTOCOL_READING_HEADER_2 1
#define ALIOLI_PROTOCOL_READING_KIND 2
#define ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_1 3
#define ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_2 4
#define ALIOLI_PROTOCOL_READING_PAYLOAD 5
#define ALIOLI_PROTOCOL_READING_CRC 6
#define ALIOLI_PROTOCOL_READING_DONE 7

// All fields except payload which is dynamic
#define ALIOLI_PROTOCOL_SIZE_HEADER FIELD_SIZEOF(AlioliProtocol, header)
#define ALIOLI_PROTOCOL_SIZE_KIND FIELD_SIZEOF(AlioliProtocol, kind)
#define ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE FIELD_SIZEOF(AlioliProtocol, payload_size)
#define ALIOLI_PROTOCOL_SIZE_CRC FIELD_SIZEOF(AlioliProtocol, crc)
#define ALIOLI_PROTOCOL_SIZE_BASE ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE + ALIOLI_PROTOCOL_SIZE_CRC
#define ALIOLI_PROTOCOL_SIZE_HEARTBEAT ALIOLI_PROTOCOL_SIZE_BASE + sizeof(HeartBeat)
#define ALIOLI_PROTOCOL_SIZE_ENVIRONMENT ALIOLI_PROTOCOL_SIZE_BASE + sizeof(Environment)
#define ALIOLI_PROTOCOL_SIZE_USERREQUEST ALIOLI_PROTOCOL_SIZE_BASE + sizeof(UserRequest)

#define ALIOLI_PROTOCOL_MAX_SIZE 512


// === General protocol fields ===

typedef struct TAlioliProtocol {
    uint16_t header;
    uint8_t kind;   // This field is attached to CRC8, one byte will be used in CRC
    uint16_t payload_size;
    byte *payload;
    byte crc;
} AlioliProtocol;

typedef struct TAlioliProtocolStatus {
    unsigned short int status;
    uint16_t total;
} AlioliProtocolStatus;


// === Basic communication ===
//
typedef struct THeartBeat {
    unsigned long int requested;
    unsigned long int answered;
} HeartBeat;

// === Control ===

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


// === GPS ===

struct TGPS {
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float course;
    unsigned long int epoch;
};
typedef struct TGPS GPS;


// === Environment ===

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

typedef struct TEnvironment {
    float altitude;
    float pressure;
    float temperaturegy;    // Temperature Gyroscope
    float temperature1;     // Sensor
    float temperature2;     // Sensor
    float temperaturebmp;   // BMP Sensor
    float voltage;          // INA Sensor (Power sensor)
    float amperage;         // INA Sensor (Power sensor)
    Acelerometer acelerometer;
    WaterAnalisys analisys;

    // Extra controls
    unsigned short int error_code;
} Environment;


// Pack structures to bytes
byte* protocol_pack_heartbeat(HeartBeat *heartbeat);
byte* protocol_pack_environment(Environment *environment);
byte* protocol_pack_userrequest(UserRequest *userrequest);


// Unpack structures from Package
unsigned short int protocol_unpack_heartbeat(AlioliProtocol *package, HeartBeat *heartbeat);
unsigned short int protocol_unpack_environment(AlioliProtocol *package, Environment *environment);
unsigned short int protocol_unpack_userrequest(AlioliProtocol *package, UserRequest *userrequest);

// Setup functions
void protocol_setup_environment(Environment *environment);
void protocol_setup_userrequest(UserRequest *userrequest);


// Parse byte-by-byte an Alioli Package
unsigned short int protocol_parse_char(byte element, AlioliProtocol *package, AlioliProtocolStatus *status);

#endif
