#ifndef MONITOR_H
#define MONITOR_H

struct TMonitorLight {
#ifdef DEBUG_MONITOR
    char name[10];
#endif
    long int nextevent;
    unsigned short int on;
    unsigned short int port;
    unsigned short int on_status;
    unsigned short int off_status;
};
typedef struct TMonitorLight MonitorLight;

struct TMonitorconfig {
    unsigned short int red_counter;
    MonitorLight onboard;
    MonitorLight red;
    MonitorLight green;
};
typedef struct TMonitorconfig MonitorConfig;

void lights_reset(long int now);
void lights_dance(long int now);
void lights_warn(long int now);
void lights_error(const char *error, const char *file, int line);

void light_set_on(MonitorLight *light, long int duration, long int now);
void light_set_off(MonitorLight *light, long int duration, long int now);

#endif
