#ifndef GYROSCOPE_H
#define GYROSCOPE_H

struct TGyroscopeConfig {
    long int nextevent;
    long int lasttime;
    unsigned short int mpu_enabled;
};
typedef struct TGyroscopeConfig GyroscopeConfig;

void gyroscope_setup(long int now);
void gyroscope_loop(long int now);

#endif
