#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "alioli.h"

// Protocol definition
// [HEADER: 2 bytes] [PAYLOAD_SIZE] [PAYLOAD] [CRC: 2 bytes]

// Protocol header
#define ALIOLI_PROTOCOL_MAGIC_HEADER 911
// #define ALIOLI_PROTOCOL_ALIGNMENT_TEST 0
// #define ALIOLI_PROTOCOL_DEBUG_DEEP 0

// Package kind
#define ALIOLI_PROTOCOL_KIND_HEARTBEAT 1
#define ALIOLI_PROTOCOL_KIND_ENVIRONMENT 2
#define ALIOLI_PROTOCOL_KIND_USERREQUEST 3

// Protocol reading status
#define ALIOLI_PROTOCOL_READING_NODATA 0
#define ALIOLI_PROTOCOL_READING_HEADER_2 1
#define ALIOLI_PROTOCOL_READING_COUNTER 2
#define ALIOLI_PROTOCOL_READING_KIND 3
#define ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_1 4
#define ALIOLI_PROTOCOL_READING_PAYLOAD_SIZE_2 5
#define ALIOLI_PROTOCOL_READING_PAYLOAD 6
#define ALIOLI_PROTOCOL_READING_CRC 7

// All fields except payload which is dynamic
#define ALIOLI_PROTOCOL_SIZE_HEADER FIELD_SIZEOF(AlioliProtocol, header)
#define ALIOLI_PROTOCOL_SIZE_COUNTER FIELD_SIZEOF(AlioliProtocol, counter)
#define ALIOLI_PROTOCOL_SIZE_KIND FIELD_SIZEOF(AlioliProtocol, kind)
#define ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE FIELD_SIZEOF(AlioliProtocol, payload_size)
#define ALIOLI_PROTOCOL_SIZE_CRC FIELD_SIZEOF(AlioliProtocol, crc)
#define ALIOLI_PROTOCOL_SIZE_PREBASE ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_COUNTER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE
#define ALIOLI_PROTOCOL_SIZE_BASE ALIOLI_PROTOCOL_SIZE_HEADER + ALIOLI_PROTOCOL_SIZE_COUNTER + ALIOLI_PROTOCOL_SIZE_KIND + ALIOLI_PROTOCOL_SIZE_PAYLOAD_SIZE + ALIOLI_PROTOCOL_SIZE_CRC
#define ALIOLI_PROTOCOL_SIZE_HEARTBEAT ALIOLI_PROTOCOL_SIZE_BASE + sizeof(HeartBeat)
#define ALIOLI_PROTOCOL_SIZE_ENVIRONMENT ALIOLI_PROTOCOL_SIZE_BASE + sizeof(Environment)
#define ALIOLI_PROTOCOL_SIZE_USERREQUEST ALIOLI_PROTOCOL_SIZE_BASE + sizeof(UserRequest)

#define ALIOLI_PROTOCOL_MAX_SIZE 512

// === General protocol fields ===

typedef struct TAlioliProtocol {
    uint16_t header;
    uint8_t counter;
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
    int32_t requested;
    int32_t answered;
} HeartBeat;

// === Control ===

typedef struct TUserRequest {
    int16_t x;                   // X-axis  PS4 Pad 2   (down, up)      =>  (-1000, +1000)
    int16_t y;                   // Y-axis  PS4 Pad 2   (left, right)   =>  (-1000, +1000)
    int16_t z;                   // Z-axis  PS4 Pad 1   (down, up)      =>  (    0, +1000)
    int16_t r;                   // R-axis  PS4 Pad 1   (left, right)   =>  (-1000, +1000)
    uint16_t buttons1;          // Buttons 1 :: Bits [but15, but14, ..., but1]
    uint16_t buttons2;          // Buttons 2 :: Bits [but31, but30, ..., but16]
    int8_t protocol_alignment_1;
    uint8_t extension;   // Extensions enabled:   0:pitch    1:roll
    int16_t pitch;               // Pitch [-1000, +1000]
    int16_t roll;                // Roll  [-1000, +1000]
} UserRequest;


// === GPS ===

struct TGPS {
    float latitude;
    float longitude;
    float altitude;
    float speed;
    float course;
    uint32_t epoch;
};
typedef struct TGPS GPS;


// === Environment ===

typedef struct TAcelerometer {
    // int AcX;
    // int AcY;
    // int AcZ;
    int8_t protocol_alignment_1;
    int8_t protocol_alignment_2;
    int8_t protocol_alignment_3;
    int8_t Tmp;
    // int GyX;
    // int GyY;
    // int GyZ;
    // float w;            // Quaternion 1, w
    // float x;            // Quaternion 2, x
    // float y;            // Quaternion 3, y
    // float z;            // Quaternion 4, z
    float roll;         // Roll [rad] (-pi..+pi)
    float pitch;        // Pitch [rad] (-pi..+pi)
    float yaw;          // Yaw [rad] (-pi..+pi)
    float compass;      // Compass [deg360] (0..360)
    float roll_speed;   // Roll angular speed [rad/s]
    float pitch_speed;  // Pitch angular speed [rad/s]
    float yaw_speed;    // Heading or Yaw angular speed [rad/s]
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
    float temperature1;     // Battery Temperature
    float temperature2;     // Water Temperature
    float temperaturebmp;   // Internal Temperature (BMP Sensor)
    float voltage;          // INA Sensor (Power sensor)
    float amperage;         // INA Sensor (Power sensor)
    Acelerometer acelerometer;
    WaterAnalisys analisys;

    // Extra controls
    int8_t protocol_alignment_1;
    int8_t protocol_alignment_2;
    int8_t protocol_alignment_3;
    uint8_t error_code;
} Environment;

// Debugging functions
void protocol_alignment_test();
#if ALIOLI_PROTOCOL_DEBUG
const char * protocol_package_kind(uint8_t kind);
void protocol_print_heartbeat(HeartBeat *heartbeat);
void protocol_print_environment(Environment *environment);
void protocol_print_userrequest(UserRequest *userrequest);
#endif

// Pack package to string
unsigned short int protocol_pack(AlioliProtocol *package, char **answer, size_t *answer_size, size_t *answer_allocated);
unsigned short int protocol_new_package(uint8_t counter, uint8_t kind, uint16_t payload_size, byte *payload, char **answer, size_t *answer_size, size_t *answer_allocated);
unsigned short int protocol_pack_heartbeat(HeartBeat *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated);
unsigned short int protocol_pack_environment(Environment *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated);
unsigned short int protocol_pack_userrequest(UserRequest *data, uint8_t counter, char **answer, size_t *answer_size, size_t *answer_allocated);

// Unpack structures from Package
unsigned short int protocol_unpack_heartbeat(AlioliProtocol *package, HeartBeat *heartbeat);
unsigned short int protocol_unpack_environment(AlioliProtocol *package, Environment *environment);
unsigned short int protocol_unpack_userrequest(AlioliProtocol *package, UserRequest *userrequest);

// Setup functions
void protocol_setup_environment(Environment **environment);
void protocol_setup_userrequest(UserRequest **userrequest);
void protocol_setup_package(AlioliProtocol *package);
void protocol_setup_status(AlioliProtocolStatus *status);

// Parse byte-by-byte an Alioli Package
unsigned short int protocol_parse_char(char element, AlioliProtocol *package, AlioliProtocolStatus *status);

#endif
