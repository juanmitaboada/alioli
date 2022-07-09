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
    print_debug(TS, stdout, CPURPLE, COLOR_NORMAL, "INI");

    // Prepare access to sensor
    OneWire oneWireObject(PINOUT_ONEWIRE_BUS_pin);
    DallasTemperature sensorDS18B20(&oneWireObject);

    // Startup sensor
    sensorDS18B20.begin(); 

    // Set environment
    rov.environment.temperature1 = 0.0;
    rov.environment.temperature2 = 0.0;

    // Set local config
    temperature_config.nextevent=0;

    print_debug(TS, stdout, CPURPLE, COLOR_NORMAL, "DONE");
}

// === LOOP === ================================================================================

void temperature_loop(long int now) {
    float temperature1=0.0, temperature2=0.0;
#if DEBUG_SENSORS_TEMPERATURE
    char s1[20]="", s2[20]="";
#endif

    // Check temperature lookup
    if (temperature_config.nextevent<now) {

        // Set next event
        temperature_config.nextevent = now+TEMPERATURE_LOOKUP_MS;

        // Instance to OneWire and DallasTemperature classes$
        OneWire oneWireObject(PINOUT_ONEWIRE_BUS_pin);
        DallasTemperature sensorDS18B20(&oneWireObject);
        DeviceAddress temp1 = TEMPERATURE_SENSOR_1;
        DeviceAddress temp2 = TEMPERATURE_SENSOR_2;

        // Get temperature from the sensors
        sensorDS18B20.requestTemperatures();
        // temperature = sensorDS18B20.getTempCByIndex(0);
        temperature1 = sensorDS18B20.getTempC(temp1);
        temperature2 = sensorDS18B20.getTempC(temp2);
        // temperature== -127.01 ===> ERROR

        // Save temperature
        rov.environment.temperature1 = temperature1;
        rov.environment.temperature2 = temperature2;

#if DEBUG_SENSORS_TEMPERATURE
        print_debug(TL, stdout, CYELLOW, COLOR_NORMAL, "Temperature 1: %sºC - Temperature 2: %sºC", dtostrf(temperature1, 8, 4, s1), dtostrf(temperature2, 8, 4, s2));
#endif
    }

}
