// Arduino internal library
#include <Wire.h>

// External libraries
// #include <DS3231.h>             // RTC - Real Time Clock
#if RTC
#include <RTClib.h>             // RTC - Real Time Clock
#endif
// #include <AsyncTaskLib.h>       // AsyncTask
// #include <PID_v1.h>             // PID Controller
// #include <MedianFilterLib.h>    // Median Filter
// #include <AsyncServoLib.h>      // Async Servo
#include <DallasTemperature.h>  // Temperature Sensor
#include <OneWire.h>            // Temperature Sensor
#ifdef GYROSCOPE_MPU
#include <SPI.h>                // MPU Sensor
#include <MPU6050.h>            // MPU Sensor
#endif
#include <Adafruit_Sensor.h>    // BNO055 - Adafruit Sensor
#include <Adafruit_BNO055.h>    // BNO055 - Adafruit BNO055
#include <Adafruit_BMP280.h>    // BMP280 - Adafruit BMP280
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_INA219.h>    // INA219
// #include <INA219.h>    // INA219

// Aliolirov libraries
#include "lib/shared.h"
#include "lib/common/alioli.ino"
#include "lib/common/serial.ino"
#include "control/control.ino"
#include "sensors/sensors.ino"
#include "sensors/analisys.ino"
#include "sensors/gyroscope.ino"
#include "sensors/power.ino"
#include "sensors/pressure.ino"
#include "sensors/temperature.ino"
#include "brain/brain.ino"
#include "engines/engines.ino"
#include "monitor/monitor.ino"
#include "transmission/communication.ino"
#include "transmission/transmission.ino"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

int led_builtin_ts=0;
unsigned short int led_builtin_status=0;

void setup() {
    DateTime datetime;
    unsigned long int now=0;

    // Boot wait
    delay(3000);

    // Header
    Serial.begin(115200);
    Serial.println("Starting bootloader... (if stuck here is because the CLOCK is not available!)");

    // Starutp Internal LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Prepare buffers
#ifdef ARDUINO_ARCH_AVR
    // Prepare buffers
    fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &serial_stdout;
    stderr = &serial_stdout;
#else
    stdout = NULL;
    stderr = NULL;
#endif

    // Show header
    print_debug("SETUP", stdout, CBLUE, COLOR_NORTC_NORMAL, "%s v%s (Build: %s - %s)", SYSNAME, VERSION, BUILD_VERSION, BUILD_DATE);
    delay(1000);

    // Detect clock
#if RTC
    print_debug("SETUP", stdout, CCYAN, COLOR_NORTC_NORMAL, "Waiting for CLOCK to be available!");
    while (!rtc.begin()) {
        print_debug("SETUP", stdout, CRED, COLOR_NORTC_NORMAL, "Waiting for CLOCK to be available!");
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(1000);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        if (led_builtin_status) {
            digitalWrite(LED_BUILTIN, LOW);
        } else {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        led_builtin_status = !led_builtin_status;
    }

    if (rtc.lostPower()) {
        // Set datetime to compilation date time
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    // Set millis offset
    rtc_millis_offset = millis();
#else
    print_debug("SETUP", stdout, CYELLOW, 0, "CLOCK not configured in your program!");
    delay(3000);
#endif

    // Get now
    now = millis();

    // Show header
    print_debug("SETUP", stdout, CBLUE, COLOR_NORMAL, "Bootloader ready (Now: %ld)", now);
    print_debug("SETUP", stdout, CCYAN, COLOR_NORMAL, "    Alioli library v%s", alioli_version());
    monitor_setup(now);
    transmission_setup(now);
    control_setup(now);
    sensors_setup(now);
    brain_setup(now);
    engines_setup(now);

    print_debug("SETUP", stdout, CGREEN, COLOR_NORMAL, "Say Hello!");
    lights_dance(now);

    print_debug("SETUP", stdout, CGREEN, COLOR_NORMAL, "Ready");

    // Setup Internal LED
    digitalWrite(LED_BUILTIN, LOW);
    led_builtin_ts = millis();
    led_builtin_status = 0;
}

void loop() {
    unsigned long int now = millis();


#if DEBUG_MAIN
    print_debug("MAIN", stdout, CCYAN, COLOR_NORMAL, "START");
#endif

    // Show Internal LED flashing
    if ((millis()-led_builtin_ts)<1000) {
        // Update timestamp
        led_builtin_ts = millis();
        // Switch led
        if (led_builtin_status) {
            digitalWrite(LED_BUILTIN, LOW);
        } else {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        led_builtin_status = !led_builtin_status;
    }

    // === CONTROL ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Control");
#endif
    control_loop(now);

    // === SENSORS ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Sensors");
#endif
    sensors_loop(now);

    // === TRANSMISSION ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Transimission");
#endif
    transmission_loop(now);

    // === BRAIN ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Brain");
#endif
    brain_loop(now);

    // === ENGINES ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Engines");
#endif
    engines_loop(now);

    // === MONITOR (always last) ===
    // This should be the last one
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Monitor");
#endif
    monitor_loop(now); // ALWAYS LAST!


#if DEBUG_MAIN
    print_debug("MAIN", stdout, CCYAN, COLOR_NORMAL, "END");
#endif

    // Loop control
    yield();
    delay(10);
}
