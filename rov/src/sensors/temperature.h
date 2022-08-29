#ifndef TEMPERATURE_H
#define TEMPERATURE_H

struct TTemperatureConfig {
    long int nextevent;
};
typedef struct TTemperatureConfig TemperatureConfig;

void temperature_setup(long int now);
void temperature_loop(long int now);

#endif
