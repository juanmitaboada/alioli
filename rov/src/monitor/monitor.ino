#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "monitor.h"

MonitorConfig monitor_config;

void light_set_on(MonitorLight *light, long int duration, long int now) {
    digitalWrite(light->port, light->on_status);
    light->on = 1;
    light->nextevent = now+duration;
}
void light_set_off(MonitorLight *light, long int duration, long int now) {
    digitalWrite(light->port, light->off_status);
    light->on = 0;
    light->nextevent = now+duration;
}

void lights_reset(long int now) {
    light_set_off(&monitor_config.onboard, ONBOARD_LIGHT_OFF_MS, now);
    light_set_off(&monitor_config.red, RED_LIGHT_OFF_MS, now);
    light_set_off(&monitor_config.green, GREEN_LIGHT_OFF_MS, now);
}

void lights_dance(long int now) {
    int i=0, lighting=50;
    for (i=0; i<5; i++) {
        // On board
        light_set_on(&monitor_config.onboard, -1, now);
        delay(lighting);
        light_set_off(&monitor_config.onboard, -1, now);
        // Red
        light_set_on(&monitor_config.red, -1, now);
        delay(lighting);
        light_set_off(&monitor_config.red, -1, now);
        // On board
        light_set_on(&monitor_config.onboard, -1, now);
        delay(lighting);
        light_set_off(&monitor_config.onboard, -1, now);
        // Green
        light_set_on(&monitor_config.green, -1, now);
        delay(lighting);
        light_set_off(&monitor_config.green, -1, now);
    }
    // Reset lights
    lights_reset(now);
}

void lights_warn(long int now) {
    int i=0;
    for (i=0; i<5; i++) {
        light_set_on(&monitor_config.onboard, -1, now);
        light_set_on(&monitor_config.red, -1, now);
        delay(50);
        light_set_off(&monitor_config.onboard, -1, now);
        light_set_off(&monitor_config.red, -1, now);
        delay(50);
    }
}

void lights_error(const char *error, const char *file, int line) {
    long int now=millis();
    while (1) {
        light_set_on(&monitor_config.onboard, -1, now);
        light_set_off(&monitor_config.green, -1, now);
        light_set_on(&monitor_config.red, -1, now);
        delay(1000);
        light_set_off(&monitor_config.onboard, -1, now);
        light_set_off(&monitor_config.red, -1, now);
        light_set_on(&monitor_config.green, -1, now);
        delay(1000);
        print_debug("TERMINAL ERROR", stderr, CRED, 0, "%s:%d: ERROR: %s", file, line, error);
    }
}

int switch_light(MonitorLight *light, unsigned long int on, unsigned long int off, long int now) {
    int lighted=0;
    if (light->nextevent<now) {
        if (light->on) {
            // ON -> OFF
            light_set_off(light, off, now);
#if DEBUG_MONITOR
            print_debug("MONITOR-LOOP", stdout, CSIMPLEGREY, COLOR_NOHEAD, "%s - Switch OFF for %ld (Now: %ld)", light->name, off, now);
#endif
        } else {
            // OFF -> ON
            light_set_on(light, on, now);
            lighted=1;
#if DEBUG_MONITOR
            print_debug("MONITOR-LOOP", stdout, CSIMPLEGREY, COLOR_NOHEAD, "%s - Switch ON for %ld (Now: %ld)", light->name, on, now);
#endif
        }
    }
    return lighted;
}

void monitor_setup(long int now) {
#if DEBUG_MONITOR
    print_debug("MONITOR-SETUP", stdout, CPURPLE, COLOR_NOTAIL, "INI -> ");
#endif

    // Set up mode
    pinMode(ONBOARD_LIGHT_PINOUT, OUTPUT);
    pinMode(RED_LIGHT_PINOUT, OUTPUT);    // Red light
    pinMode(GREEN_LIGHT_PINOUT, OUTPUT);  // Green light

    // Remember configuration
    monitor_config.onboard.port = ONBOARD_LIGHT_PINOUT;
    monitor_config.red.port = RED_LIGHT_PINOUT;
    monitor_config.green.port = GREEN_LIGHT_PINOUT;
    monitor_config.red_counter = 0;

    // Set status
    monitor_config.onboard.on_status = HIGH;
    monitor_config.onboard.off_status = LOW;
    monitor_config.red.on_status = LOW;
    monitor_config.red.off_status = HIGH;
    monitor_config.green.on_status = LOW;
    monitor_config.green.off_status = HIGH;

    // Reset lights
    lights_reset(now);

#if DEBUG_MONITOR
    // Name lights
    sprintf(monitor_config.onboard.name, "ONBOARD");
    sprintf(monitor_config.red.name, "RED");
    sprintf(monitor_config.green.name, "GREEN");
    // Show we are done
    print_debug("MONITOR-SETUP", stdout, CGREEN, COLOR_NOHEAD, "DONE");
#endif
}

void monitor_loop(long int now) {
#if LIGHTS
    unsigned short int red_on=0;

    // Switch lights
    switch_light(&monitor_config.onboard, ONBOARD_LIGHT_ON_MS, ONBOARD_LIGHT_OFF_MS, now);
    red_on = switch_light(&monitor_config.red, RED_LIGHT_ON_MS, RED_LIGHT_OFF_MS, now);
    switch_light(&monitor_config.green, GREEN_LIGHT_ON_MS, 10000, now);
    if (red_on) {
        if (monitor_config.red_counter == 2) {
            light_set_off(&monitor_config.green, GREEN_LIGHT_OFF_MS, now);
            monitor_config.red_counter = 0;
        }
        monitor_config.red_counter++;
    }
#endif

}
