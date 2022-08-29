#ifndef PRESSURE_H
#define PRESSURE_H

// IMPORTANT: You must edit Adafruit_BMP280.h and change:
// #define BMP280_ADDRESS (0x77)
// to:
// #define BMP280_ADDRESS (0x76)

struct TPressureConfig {
    long int nextevent;
};
typedef struct TPressureConfig PressureConfig;

void pressure_setup(long int now);
void pressure_loop(long int now);

#endif
