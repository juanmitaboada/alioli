#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

// === TEMPERATURE SENSOR ===
#include <OneWire.h>
#include <DallasTemperature.h>

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
    OneWire oneWireObject(PINOUT_ONEWIRE_BUS_pin);
    DallasTemperature sensorDS18B20(&oneWireObject);

    // Startup sensor
    sensorDS18B20.begin(); 

    // Set environment
    rov.environment.temp_main_battery = 0.0;
    rov.environment.temp_sea_water = 0.0;
    rov.environment.temp_engines_battery = 0.0;

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
    float temp_main_battery=0.0, temp_sea_water=0.0, temp_engines_battery=0.0;

    // Check temperature lookup
    if (temperature_config.nextevent<now) {

        // Set next event
        temperature_config.nextevent = now+TEMPERATURE_LOOKUP_MS;

        // Instance to OneWire and DallasTemperature classes$
        OneWire oneWireObject(PINOUT_ONEWIRE_BUS_pin);
        DallasTemperature sensorDS18B20(&oneWireObject);
        DeviceAddress dev_temp_main_battery = TEMPERATURE_MAIN_BATTERY;
        DeviceAddress dev_temp_engines_battery = TEMPERATURE_ENGINES_BATTERY;
        DeviceAddress dev_temp_sea_water = TEMPERATURE_SEA_WATER;

        // Get temperature from the sensors
        sensorDS18B20.requestTemperatures();
        // temperature = sensorDS18B20.getTempCByIndex(0);
        temp_main_battery = sensorDS18B20.getTempC(dev_temp_main_battery);
        temp_engines_battery = sensorDS18B20.getTempC(dev_temp_engines_battery);
        temp_sea_water = sensorDS18B20.getTempC(dev_temp_sea_water);
        // temperature== -127.01 ===> ERROR

        // Save temperature
        rov.environment.temp_main_battery = temp_main_battery;
        rov.environment.temp_engines_battery = temp_engines_battery;
        rov.environment.temp_sea_water = temp_sea_water;

#if DEBUG_SENSORS_TEMPERATURE
        char s1[20]="", s2[20]="", s3[20]="";
        print_debug(TL, stdout, CYELLOW, COLOR_NORMAL, "Temp Main Bat: %sºC - Temp Sea Water: %sºC - Temp Engines: %sºC", dtostrf(temp_main_battery, 8, 4, s1), dtostrf(temp_sea_water, 8, 4, s2), dtostrf(temp_engines_battery, 8, 4, s3));
#endif
    }

}
