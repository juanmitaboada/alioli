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

#include "pressure.h"

PressureConfig pressure_config;
const char *PRESSURE_SETUP="PS";
const char *PRESSURE_LOOP="PLS";

// === SETUP === ===============================================================================
void pressure_setup(long int now) {
    int found=0, counter=0;
    print_debug(PRESSURE_SETUP, stdout, CPURPLE, COLOR_NORMAL, "INI");

    // Initialize BMP
    counter=0;
    found=0;
    found = bmp.begin(PRESSURE_SENSOR_ADDRESS);
    while (!found) {
        print_debug("BMP-SETUP", stdout, CPURPLE, 0, "Could not find a valid BMP sensor, check wiring or I2C ADDR!");
        delay(500);
        counter++;
        if (counter<10) {
            found = bmp.begin();
        } else {
            break;
        }
    }
    if (!found) {
        // Close in a loop just in case (but lights_error is already a closed bucle)
        while (1) {
            lights_error("Could not find a valid BMP sensor, we won't continue!", __FILE__, __LINE__);
        }
    }

    // Set sampling
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,       // Operational mode
                    Adafruit_BMP280::SAMPLING_X2,       // Temp oversampling
                    Adafruit_BMP280::SAMPLING_X16,      // Pressure oversampling
                    Adafruit_BMP280::FILTER_X16,        // Filter
                    Adafruit_BMP280::STANDBY_MS_500);   // Standby time

    // Set environment
    rov.environment.pressure = 0.0;
    rov.environment.temperaturebmp = 0.0;

    // Set local config
    pressure_config.nextevent=0;

    print_debug(PRESSURE_SETUP, stdout, CPURPLE, COLOR_NORMAL, "DONE");
}

// === LOOP === ================================================================================

void pressure_loop(long int now) {
    float temperaturebmp=0.0, pressure=0.0, altitude=0.0;
#if DEBUG_SENSORS_PRESSURE
    char s1[20]="", s2[20]="", s3[20]="";
#endif

    // Check pressure lookup
    if (pressure_config.nextevent<now) {

        // Set next event
        pressure_config.nextevent = now+PRESSURE_LOOKUP_MS;

        // Read BMP sensor
        temperaturebmp = bmp.readTemperature();
        pressure = bmp.readPressure();
        altitude = bmp.readAltitude(1013.25);

        // Save info
        rov.environment.temperaturebmp = temperaturebmp;
        rov.environment.pressure = pressure;
        rov.environment.altitude = altitude;

#if DEBUG_SENSORS_PRESSURE
        print_debug(PRESSURE_LOOP, stdout, CYELLOW, COLOR_NORMAL, "Temperature BMP: %sºC - Pressure: %s Pa - Altitude: %s m", dtostrf(temperaturebmp, 8, 4, s1), dtostrf(pressure, 8, 4, s2), dtostrf(altitude, 8, 4, s3));
#endif
    }

}
