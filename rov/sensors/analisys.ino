#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

// IMPORTANT: You must edit Adafruit_BMP280.h and change:
// #define BMP280_ADDRESS (0x77)
// to:
// #define BMP280_ADDRESS (0x76)

// === TEMPERATURE SENSOR ===
#include <OneWire.h>
#include <DallasTemperature.h>

#include "analisys.h"

AnalisysConfig analisys_config;

const char *AS = "AS";
const char *AL = "AL";

// === SETUP === ===============================================================================
void analisys_setup(long int now) {
    print_debug(AS, stdout, CPURPLE, COLOR_NORMAL, "INI");

    // Setup
    pinMode(ANALISYS_PH, INPUT);
    pinMode(ANALISYS_PH_TEMP, INPUT);
    pinMode(ANALISYS_ORP, INPUT);
    pinMode(ANALISYS_ORP_TEMP, INPUT);
    pinMode(ANALISYS_CONDUCTIVITY, INPUT);
    pinMode(ANALISYS_TURBIDITY, INPUT);

    // Set environment
    rov.environment.analisys.ph = 0.0;
    rov.environment.analisys.ph_temp = 0.0;
    rov.environment.analisys.orp = 0.0;
    rov.environment.analisys.orp_temp = 0.0;
    rov.environment.analisys.conductivity = 0.0;
    rov.environment.analisys.turbidity = 0.0;

    // Set local config
    analisys_config.nextevent=0;

    print_debug(AS, stdout, CPURPLE, COLOR_NORMAL, "DONE");
}

// === LOOP === ================================================================================

void analisys_loop(long int now) {
#if DEBUG_SENSORS_ANALISYS
    char s1[20]="", s2[20]="", s3[20]="", s4[20]="", s5[20]="", s6[20]="";
#endif

    // Check analisys lookup
    if (analisys_config.nextevent<now) {

        // Set next event
        analisys_config.nextevent = now+ANALISYS_LOOKUP_MS;

        // Read sensors
        rov.environment.analisys.ph = analogRead(ANALISYS_PH);
        rov.environment.analisys.ph_temp = analogRead(ANALISYS_PH_TEMP);
        rov.environment.analisys.orp = analogRead(ANALISYS_ORP);
        rov.environment.analisys.orp_temp = analogRead(ANALISYS_ORP_TEMP);
        rov.environment.analisys.conductivity = analogRead(ANALISYS_CONDUCTIVITY);
        rov.environment.analisys.turbidity = analogRead(ANALISYS_TURBIDITY);

#if DEBUG_SENSORS_ANALISYS
        print_debug(AL, stdout, CYELLOW, COLOR_NORMAL, "pH: %s (%sºC) - ORP: %s (%sºC) - Conductivity: %s - Turbidity: %s", dtostrf(rov.environment.analisys.ph, 8, 4, s1), dtostrf(rov.environment.analisys.ph_temp, 8, 4, s2), dtostrf(rov.environment.analisys.orp, 8, 4, s3), dtostrf(rov.environment.analisys.orp_temp, 8, 4, s4), dtostrf(rov.environment.analisys.conductivity, 8, 4, s5), dtostrf(rov.environment.analisys.turbidity, 8, 4, s6));
#endif
    }

}
