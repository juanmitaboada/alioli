#ifndef POWER_H
#define POWER_H

struct TPowerConfig {
    long int nextevent;
    int ina219_main_enabled;
    int ina219_external_enabled;
};
typedef struct TPowerConfig PowerConfig;

void power_setup(long int now);
void power_loop(long int now);

#endif
