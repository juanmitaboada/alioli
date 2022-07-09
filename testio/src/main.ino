#define EXECUTE_TEST

#ifdef EXECUTE_TEST

// === Write here your test === ======================================================

#include "Arduino.h"

#ifdef ARDUINO_RASPBERRY_PI_PICO
UART Serial2(8, 9, 0, 0);
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("FORMAT ME!");
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    Serial1.begin(115200);
    Serial2.begin(115200);
}

void loop() {
    int i=0;
    for (i=0; i<10; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.println("SERIAL - No test here -> FORMAT ME!!!");
    Serial1.println("SERIAL1 - No test here -> FORMAT ME!!!");
    Serial2.println("SERIAL2 - No test here -> FORMAT ME!!!");
}


/*
int IN5 = 5;    // Input3 conectada al pin 5
int IN4 = 4;    // Input4 conectada al pin 4 
int ENB = 3;    // ENB conectada al pin 3 de Arduino
void setup()
{
    // Header
    Serial.begin(9600);
    Serial.println("Setup");
    pinMode (ENB, OUTPUT); 
    pinMode (IN5, OUTPUT);
    pinMode (IN4, OUTPUT);
    digitalWrite (IN5, LOW);
    digitalWrite (IN4, LOW);
    analogWrite(ENB, 255);
}
void loop() {
    Serial.println("INSIDE");
    digitalWrite (IN5, HIGH);
    digitalWrite (IN4, LOW);
    delay(2000);

    Serial.println("STOP");
    digitalWrite (IN5, LOW);
    digitalWrite (IN4, LOW);
    delay(200);

    Serial.println("OUTSIDE");
    digitalWrite (IN5, LOW);
    digitalWrite (IN4, HIGH);
    delay(2000);

    Serial.println("STOP");
    digitalWrite (IN5, LOW);
    digitalWrite (IN4, LOW);
    delay(2000);

    delay(3000);
}
*/

// === TEST FINISH HERE === ==========================================================


#else
#include "Arduino.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

void setup() {
    Serial.begin(115200);
    Serial.println("FORMAT ME!");
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    int i=0;
    for (i=0; i<10; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
    }
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.println("No test here -> FORMAT ME!!!");
}
#endif
