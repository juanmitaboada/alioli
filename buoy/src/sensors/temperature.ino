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
    OneWire oneWireObject(PINOUT_ONEWIRE_BUS_PIN);
    DallasTemperature sensorDS18B20(&oneWireObject);

    // Startup sensor
    sensorDS18B20.begin();

    // Set environment
    buoy.environment.temp_main_battery = 0.0;
    buoy.environment.temp_sea_water = 0.0;
    buoy.environment.temp_engines_battery = 0.0;

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
    const uint8_t *scrpd_raw = NULL;

    // Check temperature lookup
    if (temperature_config.nextevent<now) {

        // Set next event
        temperature_config.nextevent = now+TEMPERATURE_LOOKUP_MS;

        // Instance to OneWire and DallasTemperature classes
        OneWire oneWireObject(PINOUT_ONEWIRE_BUS_PIN);
        DallasTemperature sensorDS18B20(&oneWireObject);
        DeviceAddress temp_main_battery = TEMPERATURE_MAIN_BATTERY;
        DeviceAddress temp_sea_water = TEMPERATURE_SEA_WATER;

        // Get temperature from the sensors
        sensorDS18B20.requestTemperatures();
        buoy.environment.temp_main_battery = sensorDS18B20.getTempC(temp_main_battery);
        buoy.environment.temp_sea_water = sensorDS18B20.getTempC(temp_sea_water);
        // temperature== -127.01 ===> ERROR

        print_debug(TL, stdout, CYELLOW, COLOR_NORMAL, "MAIN BATERY: %.2fºC - SEA WATER: %.2fºC", buoy.environment.temp_main_battery, buoy.environment.temp_sea_water);
#if DEBUG_SENSORS_TEMPERATURE
#endif
    }

}
