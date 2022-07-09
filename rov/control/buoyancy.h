#ifndef BUOYANCY_H
#define BUOYANCY_H

struct TBuoyancyConfig {
    long int nextevent;
    unsigned short int on_status;
    unsigned short int off_status;
    long int timer_in_front_left;
    long int timer_in_front_right;
    long int timer_in_back_left;
    long int timer_in_back_right;
    long int timer_out_front_left;
    long int timer_out_front_right;
    long int timer_out_back_left;
    long int timer_out_back_right;
};
typedef struct TBuoyancyConfig BuoyancyConfig;

#endif
