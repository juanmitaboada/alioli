#define EXECUTE_TEST

#ifdef EXECUTE_TEST

// === Write here your test === ======================================================

#include "Arduino.h"

#ifdef ARDUINO_RASPBERRY_PI_PICO
// UART Serial2(8, 9, 0, 0);
UART Serial2(4, 5, 0, 0);
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define TEST_PIN 3

void setup() {
    // Bootwait
    delay(3000);

    // Start job
    Serial.begin(115200);
    Serial.println("FORMAT ME!");
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
#ifdef TEST_PIN
    pinMode(TEST_PIN, OUTPUT);
#endif

    Serial1.begin(115200);
    Serial2.begin(115200);
}

void loop() {
    unsigned short int incomingByte=0, s1len=0, s2len=0;
    char s1[100]="", s2[100]="";
    int i=0;
    for (i=0; i<10; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
#ifdef TEST_PIN
        digitalWrite(TEST_PIN, HIGH);
#endif
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
#ifdef TEST_PIN
        digitalWrite(TEST_PIN, LOW);
#endif
        delay(100);
    }
    digitalWrite(LED_BUILTIN, HIGH);
#ifdef TEST_PIN
    digitalWrite(TEST_PIN, HIGH);
#endif
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
#ifdef TEST_PIN
    digitalWrite(TEST_PIN, LOW);
#endif
    delay(1000);

    // SERIAL 0
    Serial.print("SERIAL - No test here -> FORMAT ME!!!");
#ifdef TEST_PIN
    Serial.print(" :: Test PIN: ");
    Serial.print(TEST_PIN);
#endif
    while (Serial1.available()>0) {
        s1[s1len] = (char) Serial1.read();
        s1len++;
    }
    while (Serial2.available()>0) {
        s1[s2len] = (char) Serial2.read();
        s2len++;
    }
    if (s1len) {
        Serial.print(" - S1: ·");
        Serial.print(s1);
        Serial.print("·");
    }
    if (s2len) {
        Serial.print(" - S2: ·");
        Serial.print(s2);
        Serial.print("·");
    }
    Serial.println("");

    // SERIAL 1
    Serial1.print("SERIAL1 - No test here -> FORMAT ME!!!");
#ifdef TEST_PIN
    Serial1.print(" :: Test PIN: ");
    Serial1.print(TEST_PIN);
#endif
    if (s1len) {
        Serial1.print(" - DATA: ·");
        Serial1.print(s1);
        Serial1.print("·");
    }
    Serial1.println("");

    // SERIAL 2
    Serial2.print("SERIAL2 - No test here -> FORMAT ME!!!");
#ifdef TEST_PIN
    Serial2.print(" :: Test PIN: ");
    Serial2.print(TEST_PIN);
#endif
    if (s2len) {
        Serial2.print(" - DATA: ·");
        Serial2.print(s2);
        Serial2.print("·");
    }
    Serial2.println("");
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
