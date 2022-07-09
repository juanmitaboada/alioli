// Arduino internal librMAIN
#include <Arduino.h>
#include <Wire.h>

// External libraries
#if RTC
#include <RTClib.h>                 // RTC - Real Time Clock
#endif

// Buoy libraries
#include "lib/alioli.ino"
#include "transmission/transmission.ino"
#include "transmission/communication.ino"

// #include "esp_heap_trace.h"

// #define NUM_RECORDS 100
// static heap_trace_record_t trace_record[NUM_RECORDS];

int freeram=0;

void setup() {
#if RTC
    DateTime datetime;
#endif
    unsigned long int now=0;

    // Boot wait
    delay(3000);

    // Header
    Serial.begin(115200);
    Serial.println("Starting bootloader... (if stuck here is because the CLOCK is not available!)");
    Serial.setDebugOutput(true);

    // Prepare buffers
    // fdev_setup_stream(&serial_stdout, serial_putchar, NULL, _FDEV_SETUP_WRITE);
    // stdout = &serial_stdout;
    // stderr = &serial_stdout;

    // Detect clock
#if RTC
    print_debug("SETUP", stdout, CRED, COLOR_NORMAL, "Waiting for CLOCK to be available!");
    while (!rtc.begin()) {
        print_debug("SETUP", stdout, CRED, COLOR_NORMAL, "Waiting for CLOCK to be available!");
        delay(1000);
    }

    if (rtc.lostPower()) {
        // Set datetime to compilation date time
        rtc.adjust(DateTime(__DATE__, __TIME__));
    }

    // Set millis offset
    rtc_millis_offset = millis();
#else
    print_debug("SETUP", stdout, CYELLOW, COLOR_NORMAL, "CLOCK not configured in your program!");
    delay(3000);
#endif

    // Get now
    now = millis();

    // Show header
    print_debug("SETUP", stdout, CBLUE, COLOR_NORMAL, "Bootloader ready (Now: %ld)", now);
    print_debug("SETUP", stdout, CCYAN, COLOR_NORMAL, "    Alioli library v%s", alioli_version());
    transmission_setup(now);

    print_debug("SETUP", stdout, CGREEN, COLOR_NORMAL, "Say Hello!");
    print_debug("SETUP", stdout, CGREEN, COLOR_NORMAL, "Ready");
    freeram=ESP.getFreeHeap();
}

void loop() {
    unsigned long int now = millis();
    int actual_freeram=0;

#if DEBUG_MAIN
    print_debug("MAIN", stdout, CCYAN, COLOR_NORMAL, "START");
#endif

    // === TRANSMISSION ===
#if DEBUG_MAIN
    print_debug("MAIN", stdout, "purple", COLOR_NORMAL, "Transmission");
#endif
    transmission_loop(now);

#if DEBUG_MAIN
    print_debug("MAIN", stdout, CCYAN, COLOR_NORMAL, "END");
#endif

    // Verify memory leaks (memory should stay stable after every loop)
    actual_freeram = ESP.getFreeHeap();
    if (actual_freeram>freeram) {
        print_debug("MAIN", stdout, CGREEN, 0, "%s RAM detected: %d bytes (Free RAM: %d)", "FREE", actual_freeram-freeram, actual_freeram);
        freeram = actual_freeram;
    } else if (actual_freeram<freeram) {
        print_debug("MAIN", stdout, CYELLOW, 0, "%s RAM detected: %d bytes (Free RAM: %d)", "USED", freeram-actual_freeram, actual_freeram);
        freeram = actual_freeram;
    }

    delay(10);
    yield();

}
