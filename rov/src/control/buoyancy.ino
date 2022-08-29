#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "monitor/monitor.h"
#include "buoyancy.h"

BuoyancyConfig buoyancy_config;

// === SETUP === ==========================================================

void buoyancy_on(int pin, long int now, long int *timer) {
    (*timer) = now+BUOYANCY_TOKEN_MS;
    digitalWrite(pin, buoyancy_config.on_status);
}

void buoyancy_off(int pin, long int now, long int *timer) {
    if (*timer && (now>*timer)) {
        digitalWrite(pin, buoyancy_config.off_status);
        (*timer) = 0;
    }
}

void buoyancy_test(int pin, long int now) {
    digitalWrite(pin, buoyancy_config.on_status);
    delay(BUOYANCY_TOKEN_MS);
    digitalWrite(pin, buoyancy_config.off_status);
}

void buoyancy_setup(long int now) {

    // Inialize
    buoyancy_config.nextevent = 0;

    // Set on/off status
    buoyancy_config.on_status = LOW;
    buoyancy_config.off_status = HIGH;

    // Timers
    buoyancy_config.timer_in_front_left = 0;
    buoyancy_config.timer_in_front_right = 0;
    buoyancy_config.timer_in_back_left = 0;
    buoyancy_config.timer_in_back_right = 0;
    buoyancy_config.timer_out_front_left = 0;
    buoyancy_config.timer_out_front_right = 0;
    buoyancy_config.timer_out_back_left = 0;
    buoyancy_config.timer_out_back_right = 0;

    // Setup
    pinMode(BUOYANCY_IN_FRONT_LEFT, OUTPUT);
    pinMode(BUOYANCY_IN_FRONT_RIGHT, OUTPUT);
    pinMode(BUOYANCY_IN_BACK_LEFT, OUTPUT);
    pinMode(BUOYANCY_IN_BACK_RIGHT, OUTPUT);
    pinMode(BUOYANCY_OUT_FRONT_LEFT, OUTPUT);
    pinMode(BUOYANCY_OUT_FRONT_RIGHT, OUTPUT);
    pinMode(BUOYANCY_OUT_BACK_LEFT, OUTPUT);
    pinMode(BUOYANCY_OUT_BACK_RIGHT, OUTPUT);
    digitalWrite(BUOYANCY_IN_FRONT_LEFT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_IN_FRONT_RIGHT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_IN_BACK_LEFT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_IN_BACK_RIGHT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_OUT_FRONT_LEFT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_OUT_FRONT_RIGHT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_OUT_BACK_LEFT, buoyancy_config.off_status);
    digitalWrite(BUOYANCY_OUT_BACK_RIGHT, buoyancy_config.off_status);

    // Decide if to use Quick Start
    if (!QUICK_START) {
        buoyancy_test(BUOYANCY_IN_FRONT_LEFT, now);
        buoyancy_test(BUOYANCY_IN_FRONT_RIGHT, now);
        buoyancy_test(BUOYANCY_IN_BACK_LEFT, now);
        buoyancy_test(BUOYANCY_IN_BACK_RIGHT, now);
        buoyancy_test(BUOYANCY_OUT_FRONT_LEFT, now);
        buoyancy_test(BUOYANCY_OUT_FRONT_RIGHT, now);
        buoyancy_test(BUOYANCY_OUT_BACK_LEFT, now);
        buoyancy_test(BUOYANCY_OUT_BACK_RIGHT, now);
    }
}

// === LOOP === ===========================================================

void buoyancy_loop(long int now) {

    // Check buoyancy lookup
    if (buoyancy_config.nextevent<now) {

        // Update nextevent
        buoyancy_config.nextevent = now+BUOYANCY_LOOKUP_MS;

        // Check all
        buoyancy_off(BUOYANCY_IN_FRONT_LEFT, now, &buoyancy_config.timer_in_front_left);
        buoyancy_off(BUOYANCY_IN_FRONT_RIGHT, now, &buoyancy_config.timer_in_front_right);
        buoyancy_off(BUOYANCY_IN_BACK_LEFT, now, &buoyancy_config.timer_in_back_left);
        buoyancy_off(BUOYANCY_IN_BACK_RIGHT, now, &buoyancy_config.timer_in_back_right);
        buoyancy_off(BUOYANCY_OUT_FRONT_LEFT, now, &buoyancy_config.timer_out_front_left);
        buoyancy_off(BUOYANCY_OUT_FRONT_RIGHT, now, &buoyancy_config.timer_out_front_right);
        buoyancy_off(BUOYANCY_OUT_BACK_LEFT, now, &buoyancy_config.timer_out_back_left);
        buoyancy_off(BUOYANCY_OUT_BACK_RIGHT, now, &buoyancy_config.timer_out_back_right);
    }

}
