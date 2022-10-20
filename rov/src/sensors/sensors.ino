#include "lib/config.h"
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
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.print(F("SENSORS INI "));
#else
    print_debug(SENSORS_SETUP, stdout, CPURPLE, COLOR_NORMAL, "INI");
#endif
#endif
    power_setup(now);
    gyroscope_setup(now);
    temperature_setup(now);
    pressure_setup(now);
    analisys_setup(now);
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("DONE"));
#else
    print_debug(SENSORS_SETUP, stdout, CPURPLE, COLOR_NORMAL, "DONE");
#endif
#endif
}

// === LOOP === ===========================================================

void sensors_loop(long int now) {
#if MODULE_SENSORS
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("Gyroscope"));
#else
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Gyroscope");
#endif
#endif
    gyroscope_loop(now);

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("Temperature"));
#else
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Temperature");
#endif
#endif
    temperature_loop(now);

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("Pressure"));
#else
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Pressure");
#endif
#endif
    pressure_loop(now);

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("Power"));
#else
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Power");
#endif
#endif
    power_loop(now);

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("Analisys"));
#else
    print_debug(SENSORS_LOOP, stdout, CPURPLE, COLOR_NORMAL, "Analisys");
#endif
#endif
    analisys_loop(now);

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("loop end!"));
#else
    print_debug(SENSORS_LOOP, stdout, CYELLOW, COLOR_NORMAL, "loop end!");
#endif
#endif
#endif
}
