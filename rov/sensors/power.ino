#include "lib/shared.h"
#include "lib/config.h"
#include "lib/common/alioli.h"

// === TEMPERATURE SENSOR ===
#include <Wire.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_INA219.h>

#include "power.h"

PowerConfig power_config;

const char *POWER_SETUP="PS";
const char *POWER_LOOP="PL";

// === SETUP === ===============================================================================
void power_setup(long int now) {
    unsigned short int counter=0, found=0;
    print_debug(POWER_SETUP, stdout, CPURPLE, COLOR_NORMAL, "INI");

    // Initialize sensor
    counter=0;
    found=0;
    found = ina219.begin();
    while (!found) {
        print_debug(POWER_SETUP, stdout, CPURPLE, 0, "Could not find a valid INA219 sensor, check wiring or I2C ADDR!");
        delay(500);
        counter++;
        if (counter<10) {
            found = ina219.begin();
        } else {
            break;
        }

    }
    power_config.ina219_enabled = found;
    if (!found) {
#if POWER_INA219_MUST_EXIST
        // Close in a loop just in case (but lights_error is already a closed bucle)
        while (1) {
            lights_error("Could not find a valid INA219 sensor, we won't continue!", __FILE__, __LINE__);
        }
#else
        print_debug(POWER_SETUP, stdout, CPURPLE, 0, "Could not find a valid INA219 sensor, we will keep going with NO POWER SENSOR!");
#endif
    }

    // Set environment
    rov.environment.voltage = 0.0;
    rov.environment.amperage = 0.0;

    // Set local config
    power_config.nextevent=0;

    print_debug(POWER_SETUP, stdout, CPURPLE, COLOR_NORMAL, "DONE");
}

// === LOOP === ================================================================================

void power_loop(long int now) {
    float voltage=0.0, amperage=0.0;
#if DEBUG_SENSORS_POWER
    char s1[20]="", s2[20]="";
#endif

    // Check power lookup
    if (power_config.nextevent<now) {

        // Set next event
        power_config.nextevent = now+POWER_LOOKUP_MS;

#if DEBUG_SENSORS_POWER
        float shuntvoltage = 0;
        float busvoltage = 0;
        float current_mA = 0;
        float loadvoltage = 0;
        float power_mW = 0;

        shuntvoltage = ina219.getShuntVoltage_mV();
        busvoltage = ina219.getBusVoltage_V();
        current_mA = ina219.getCurrent_mA();
        power_mW = ina219.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
        Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
        Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
        Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
        Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
        Serial.println("");
#endif

        /*

Example:

...

Bus Voltage:   11.90 V
Shunt Voltage: 6.03 mV
Load Voltage:  11.91 V
Current:       60.50 mA
Power:         720.00 mW

Bus Voltage:   11.90 V
Shunt Voltage: 6.02 mV
Load Voltage:  11.91 V
Current:       60.10 mA
Power:         718.00 mW

Bus Voltage:   11.90 V
Shunt Voltage: 6.02 mV
Load Voltage:  11.91 V
Current:       60.50 mA
Power:         720.00 mW

Bus Voltage:   11.90 V
Shunt Voltage: 6.00 mV
Load Voltage:  11.91 V
Current:       59.70 mA
Power:         718.00 mW

Bus Voltage:   11.90 V
Shunt Voltage: 6.00 mV
Load Voltage:  11.91 V
Current:       60.50 mA
Power:         720.00 mW

...
           */

        // Save temperature
        rov.environment.voltage = voltage;
        rov.environment.amperage = amperage;

#if DEBUG_SENSORS_POWER
        print_debug(POWER_LOOP, stdout, CYELLOW, COLOR_NORMAL, "Temperature 1: %sºC - Temperature 2: %sºC", dtostrf(temperature1, 8, 4, s1), dtostrf(temperature2, 8, 4, s2));
#endif
    }

}
