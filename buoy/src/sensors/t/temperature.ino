#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

// === TEMPERATURE SENSOR ===
#include "temperature.h"

TemperatureConfig temperature_config;

const char *TS="TS";
const char *TL="TL";

// === SETUP === ===============================================================================
void temperature_setup(long int now) {

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.print(F("TS: INI: "));
#else
    print_debug(TS, stdout, CPURPLE, COLOR_NORMAL, "INI");
#endif
#endif

    // Prepare access to sensor
    new (&ow) OneWireNg_CurrentPlatform(PINOUT_ONEWIRE_BUS_PIN, false);
    DSTherm drv(*ow);
    drv.filterSupportedSlaves();

    // Set environment
    buoy.environment.temperature1 = 0.0;
    buoy.environment.temperature2 = 0.0;

    // Set local config
    temperature_config.nextevent=0;

#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.print(F("DONE"));
#else
    print_debug(TS, stdout, CPURPLE, COLOR_NORMAL, "DONE");
#endif
#endif
}

// === LOOP === ================================================================================

void temperature_loop(long int now) {
    Placeholder<DSTherm::Scratchpad> scrpd;
    float temperature1=0.0, temperature2=0.0;

    // Check temperature lookup
    if (temperature_config.nextevent<now) {

        // Set next event
        temperature_config.nextevent = now+TEMPERATURE_LOOKUP_MS;

        // Instance to OneWireNG and its elements
        DSTherm drv(*ow);

        // Get temperature from the sensors
        drv.convertTempAll(DSTherm::SCAN_BUS, false);
        if (drv.readScratchpad(TEMPERATURE_SENSOR_1, &scrpd) == OneWireNg::EC_SUCCESS) {
            temperature1 = scrpd->getTh();
        }
        if (drv.readScratchpad(TEMPERATURE_SENSOR_2, &scrpd) == OneWireNg::EC_SUCCESS) {
            temperature2 = scrpd->getTh();
        }
        // temperature== -127.01 ===> ERROR

        // Save temperature
        buoy.environment.temperature1 = temperature1;
        buoy.environment.temperature2 = temperature2;

#if DEBUG_SENSORS_TEMPERATURE
        print_debug(TL, stdout, CYELLOW, COLOR_NORMAL, "Temperature 1: %.2fºC - Temperature 2: %.2fºC", temperature1, temperature2);
#endif
    }

}
