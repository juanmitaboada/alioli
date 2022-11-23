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
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("POWER: INI"));
#else
    print_debug(POWER_SETUP, stdout, CPURPLE, COLOR_NORMAL, "INI");
#endif
#endif

    // === INA219 MAIN BATTERY ===
    // Initialize sensor
    counter=0;
    found=0;
    found = ina219_main.begin();
    while (!found) {
#if DEBUG_SENSORS
#if OPTIMIZE
        Serial.println(F("POWER: Couldn't find a valid INA219 MAIN sensor, check wiring or I2C ADDR!"));
#else
        print_debug(POWER_SETUP, stdout, CPURPLE, 0, "Could not find a valid INA219 MAIN sensor, check wiring or I2C ADDR!");
#endif
#endif
        delay(500);
        counter++;
        if (counter<10) {
            found = ina219_main.begin();
        } else {
            break;
        }

    }
    power_config.ina219_main_enabled = found;
    if (!found) {
#if POWER_INA219_MAIN_MUST_EXIST
        // Close in a loop just in case (but lights_error is already a closed bucle)
        while (1) {
#if OPTIMIZE
            lights_error(NULL, __FILE__, __LINE__);
#else
            lights_error("Could not find a valid INA219 MAIN sensor, we won't continue!", __FILE__, __LINE__);
#endif
        }
#else
#if DEBUG_SENSORS
#if OPTIMIZE
        Serial.println(F("POWER: Couldn't find a valid INA219 MAIN sensor, we will keep going with NO POWER SENSOR!"));
#else
        print_debug(POWER_SETUP, stdout, CPURPLE, 0, "Could not find a valid INA219 MAIN sensor, we will keep going with NO POWER SENSOR!");
#endif
#endif
#endif
    }

    // === INA219 EXTERNAL BATTERY ===
    // Initialize sensor
    counter=0;
    found=0;
    found = ina219_external.begin();
    while (!found) {
#if DEBUG_SENSORS
#if OPTIMIZE
        Serial.println(F("POWER: Couldn't find a valid INA219 EXTERNAL sensor, check wiring or I2C ADDR!"));
#else
        print_debug(POWER_SETUP, stdout, CPURPLE, 0, "Could not find a valid INA219 EXTERNAL sensor, check wiring or I2C ADDR!");
#endif
#endif
        delay(500);
        counter++;
        if (counter<10) {
            found = ina219_external.begin();
        } else {
            break;
        }

    }
    power_config.ina219_external_enabled = found;
    if (!found) {
#if POWER_INA219_EXTERNAL_MUST_EXIST
        // Close in a loop just in case (but lights_error is already a closed bucle)
        while (1) {
#if OPTIMIZE
            lights_error(NULL, __FILE__, __LINE__);
#else
            lights_error("Could not find a valid INA219 EXTERNAL sensor, we won't continue!", __FILE__, __LINE__);
#endif
        }
#else
#if DEBUG_SENSORS
#if OPTIMIZE
        Serial.println(F("POWER: Couldn't find a valid INA219 EXTERNAL sensor, we will keep going with NO POWER SENSOR!"));
#else
        print_debug(POWER_SETUP, stdout, CPURPLE, 0, "Could not find a valid INA219 EXTERNAL sensor, we will keep going with NO POWER SENSOR!");
#endif
#endif
#endif
    }

    // Set environment
    rov.environment.voltage_main = 0.0;
    rov.environment.amperage_main = 0.0;
    rov.environment.voltage_external = 0.0;
    rov.environment.amperage_external = 0.0;

    // Set local config
    power_config.nextevent=0;
#if DEBUG_SENSORS
#if OPTIMIZE
    Serial.println(F("DONE"));
#else
    print_debug(POWER_SETUP, stdout, CPURPLE, COLOR_NORMAL, "DONE");
#endif
#endif
}

// === LOOP === ================================================================================

void power_loop(long int now) {

    // Check power lookup
    if (power_config.nextevent<now) {

        // Set next event
        power_config.nextevent = now+POWER_LOOKUP_MS;

        // Get power information
        rov.environment.voltage_main = ina219_main.getBusVoltage_V()+ ( ina219_main.getShuntVoltage_mV() / 1000);
        rov.environment.amperage_main = ina219_main.getCurrent_mA();
        rov.environment.voltage_external = ina219_external.getBusVoltage_V()+ ( ina219_external.getShuntVoltage_mV() / 1000);
        rov.environment.amperage_external = ina219_external.getCurrent_mA();

#if DEBUG_SENSORS_POWER
        float shuntvoltage = 0;
        float busvoltage = 0;
        float current_mA = 0;
        float loadvoltage = 0;
        float power_mW = 0;

        // MAIN
        shuntvoltage = ina219_main.getShuntVoltage_mV();
        busvoltage = ina219_main.getBusVoltage_V();
        current_mA = ina219_main.getCurrent_mA();
        power_mW = ina219_main.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        Serial.print(F("M:Bus Voltage:   ")); Serial.print(busvoltage); Serial.println(F(" V"));
        Serial.print(F("M:Shunt Voltage: ")); Serial.print(shuntvoltage); Serial.println(F(" mV"));
        Serial.print(F("M:Load Voltage:  ")); Serial.print(loadvoltage); Serial.println(F(" V"));
        Serial.print(F("M:Current:       ")); Serial.print(current_mA); Serial.println(F(" mA"));
        Serial.print(F("M:Power:         ")); Serial.print(power_mW); Serial.println(F(" mW"));
        Serial.println("");

        // EXTERNAL
        shuntvoltage = ina219_external.getShuntVoltage_mV();
        busvoltage = ina219_external.getBusVoltage_V();
        current_mA = ina219_external.getCurrent_mA();
        power_mW = ina219_external.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        Serial.print(F("E:Bus Voltage:   ")); Serial.print(busvoltage); Serial.println(F(" V"));
        Serial.print(F("E:Shunt Voltage: ")); Serial.print(shuntvoltage); Serial.println(F(" mV"));
        Serial.print(F("E:Load Voltage:  ")); Serial.print(loadvoltage); Serial.println(F(" V"));
        Serial.print(F("E:Current:       ")); Serial.print(current_mA); Serial.println(F(" mA"));
        Serial.print(F("E:Power:         ")); Serial.print(power_mW); Serial.println(F(" mW"));
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

        /*
#if DEBUG_SENSORS
#if DEBUG_SENSORS_POWER
#if OPTIMIZE
        Serial.print(F("POWER: INI"));
#else
    char s1[20]="", s2[20]="";
    print_debug(POWER_LOOP, stdout, CYELLOW, COLOR_NORMAL, "MAIN: %sV - EXTERNAL: %sV", dtostrf(rov.environment.voltage_main, 8, 4, s1), dtostrf(rov.environment.voltage_external, 8, 4, s2));
#endif
#endif
#endif
        */
    }

}
