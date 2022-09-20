// Arduino internal librMAIN
// #include <Arduino.h>
#include <Wire.h>

// External libraries
#if RTC
#include <RTClib.h>                 // RTC - Real Time Clock
#endif

#include <OneWireNg_CurrentPlatform.h>            // Temperature Sensor
#include <DallasTemperature.h>

#include <Adafruit_Sensor.h>    // BNO055 - Adafruit Sensor
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_INA219.h>    // INA219

// Buoy libraries
#include "lib/shared.h"
#include "lib/common/alioli.ino"
#include "lib/common/serial.ino"
#include "lib/common/protocol.ino"
#include "sensors/sensors.ino"
#include "sensors/power.ino"
#include "sensors/temperature.ino"
#include "transmission/transmission.ino"
#include "transmission/communication.ino"

#ifdef ARDUINO_RASPBERRY_PI_PICO
UART Serial2(SERIAL2_TX_PIN, SERIAL2_RX_PIN, 0, 0);
// MbedI2C Wire(BUS_I2C_SDA, BUS_I2C_SCL);
// MbedI2C(BUS_I2C_SDA, BUS_I2C_SCL);
// MbedI2C Wire1(BUS_I2C_SDA, BUS_I2C_SCL);
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

int freeram=0;
int led_builtin_ts=0;
unsigned short int led_builtin_status=0;

void setup() {
#if RTC
    DateTime datetime;
#endif
    long int now=0;

    // Boot wait
    delay(3000);

    // Header
    Serial.begin(115200);
    Serial.println(F("Starting bootloader..."));

    // Protocol alignment test
    protocol_alignment_test();

    // Startup Internal LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

#ifdef ARDUINO_ARCH_AVR
    // Prepare buffers
    fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &serial_stdout;
    stderr = &serial_stdout;
#endif
    stdout = NULL;
    stderr = NULL;

    // Show header
    print_debug("SETUP", stdout, CBLUE, COLOR_NORTC_NORMAL, "%s v%s (Build: %s - %s)", SYSNAME, VERSION, BUILD_VERSION, BUILD_DATE);

    // Detect clock
#if RTC
    print_debug("SETUP", stdout, CCYAN, COLOR_NORTC_NORMAL, "Waiting for CLOCK to be available!");
    while (!rtc.begin()) {
        print_debug("SETUP", stdout, CRED, COLOR_NORTC_NORMAL, "Waiting for CLOCK to be available!");
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        if (led_builtin_status) {
            digitalWrite(LED_BUILTIN, LOW);
        } else {
            digitalWrite(LED_BUILTIN, HIGH);
        }
        led_builtin_status = !led_builtin_status;
    }

    if (rtc.lostPower()) {
        // Set datetime to compilation date time
        Serial.print(F("RTC - NO BATTERY - Setting datetime to firmware datetime: "));
        Serial.print(F(__DATE__));
        Serial.print(F(" "));
        Serial.println(F(__TIME__));
        rtc.adjust(DateTime(F(__DATE__), F( __TIME__)));
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
    // ===========
    print_debug("SETUP", stdout, CBLUE, 0, "Bootloader ready (Now: %ld)", now);
    print_debug("SETUP", stdout, CCYAN, 0, "    Alioli library v%s", alioli_version());
#ifdef ESP32
    freeram=ESP.getFreeHeap();
#endif

    // Setup main object
    // =================
    protocol_setup_environment(&buoy.environment);
    protocol_setup_userrequest(&buoy.userrequest);
    buoy.environment_newdata = 0;
    // GPS
    buoy.gps.latitude = 0.0;
    buoy.gps.longitude = 0.0;
    buoy.gps.altitude = 0.0;
    buoy.gps.speed = 0.0;
    buoy.gps.course = 0.0;
    buoy.gps.epoch = 0;
    buoy.gps_newdata = 0;
    // Acelerometer (DUMMY)
    buoy.acelerometer_newdata = 1;

    // Setup modules
    // =============
    sensors_setup(now);
    transmission_setup(now);

    // Welcome
    // =======
    print_debug("SETUP", stdout, CGREEN, 0, "Say Hello!");
    print_debug("SETUP", stdout, CGREEN, 0, "Ready");

    // Setup Internal LED
    digitalWrite(LED_BUILTIN, LOW);
    led_builtin_ts = millis();
    led_builtin_status = 0;
}

void loop() {
    long int now = millis();
    int actual_freeram=0;

#if DEBUG_MAIN
    print_debug("MAIN", stdout, CCYAN, 0, "START");
#endif

    // Show Internal LED flashing
    if ((millis()-led_builtin_ts)>1000) {
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

    // === SENSORS ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, CPURPLE, COLOR_NORMAL, "Sensors");
#endif
    sensors_loop(now);

    // === TRANSMISSION ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, "purple", 0, "Transmission");
#endif
    transmission_loop(now);

#if DEBUG_MAIN
    print_debug("MAIN", stdout, CCYAN, 0, "END");
#endif

    // Verify memory leaks (memory should stay stable after every loop)
#ifdef ESP32
    actual_freeram = ESP.getFreeHeap();
#endif
    if (actual_freeram>freeram) {
        print_debug("MAIN", stdout, CGREEN, 0, "%s RAM detected: %d bytes (Free RAM: %d)", "FREE", actual_freeram-freeram, actual_freeram);
        freeram = actual_freeram;
    } else if (actual_freeram<freeram) {
        print_debug("MAIN", stdout, CYELLOW, 0, "%s RAM detected: %d bytes (Free RAM: %d)", "USED", freeram-actual_freeram, actual_freeram);
        freeram = actual_freeram;
    }

    // Loop control
    yield();
}
