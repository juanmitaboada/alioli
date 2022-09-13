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
    const uint8_t *scrpd_raw = NULL;
    float temperature1=0.0, temperature2=0.0;

    // Check temperature lookup
    if (temperature_config.nextevent<now) {

        // Set next event
        temperature_config.nextevent = now+TEMPERATURE_LOOKUP_MS;

        // Instance to OneWire and DallasTemperature classes
        OneWire oneWireObject(PINOUT_ONEWIRE_BUS_PIN);
        DallasTemperature sensorDS18B20(&oneWireObject);
        DeviceAddress temp1 = TEMPERATURE_SENSOR_1;
        DeviceAddress temp2 = TEMPERATURE_SENSOR_2;

        // Get temperature from the sensors
        sensorDS18B20.requestTemperatures();
        temperature1 = sensorDS18B20.getTempC(temp1);
        temperature2 = sensorDS18B20.getTempC(temp2);
        // temperature== -127.01 ===> ERROR

        // Save temperature
        buoy.environment.temperature1 = temperature1;
        buoy.environment.temperature2 = temperature2;

#if DEBUG_SENSORS_TEMPERATURE
        print_debug(TL, stdout, CYELLOW, COLOR_NORMAL, "Temperature 1: %.2fºC - Temperature 2: %.2fºC", temperature1, temperature2);
#endif
    }

}
