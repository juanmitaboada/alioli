#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

#include "analisys.h"
#include "gyroscope.h"
#include "temperature.h"
#include "pressure.h"
#include "power.h"

// === SETUP === ==========================================================

const char *SENSORS_SETUP="SS";
const char *SENSORS_LOOP="SL";

void sensors_setup(long int now) {
    print_debug(SENSORS_SETUP, stdout, CPURPLE, COLOR_NORMAL, "INI");
    power_setup(now);
    gyroscope_setup(now);
    temperature_setup(now);
    pressure_setup(now);
    analisys_setup(now);
    print_debug(SENSORS_SETUP, stdout, CPURPLE, COLOR_NORMAL, "DONE");
}

// === LOOP === ===========================================================

void sensors_loop(long int now) {
    /*
#if DEBUG_SENSORS
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Gyroscope");
#endif
    gyroscope_loop(now);

#if DEBUG_SENSORS
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Temperature");
#endif
    temperature_loop(now);

#if DEBUG_SENSORS
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Pressure");
#endif
    pressure_loop(now);

#if DEBUG_SENSORS
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Power");
#endif
    power_loop(now);

#if DEBUG_SENSORS
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Analisys");
#endif
    analisys_loop(now);

#if DEBUG_SENSORS
    print_debug(SENSORS_LOOP, stdout, CYELLOW, COLOR_NORMAL, "loop end!");
#endif
    */
}
