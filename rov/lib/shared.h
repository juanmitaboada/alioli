#ifndef SHARED_H
#define SHARED_H

#include "common/alioli.h"
#include "config.h"
#include "constants.h"
#include "version.h"

FILE serial_stdout;

// === RTC ===
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;
long int rtc_millis_offset;

// === Temperature ===
#include <OneWire.h>
#include <DallasTemperature.h>

// Instance to OneWire and DallasTemperature classes
// OneWire oneWireObject(PINOUT_TEMPERATURE_pin);
// DallasTemperature sensorDS18B20(&oneWireObject);
// DeviceAddress ADDRESS_TEMPERATURE_SENSOR_1 = TEMPERATURE_SENSOR_1;

// === MPU ===

#ifdef GYROSCOPE_MPU
#include <MPU6050.h>
MPU6050 mpu;
#endif
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
Adafruit_BNO055 bno;
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;

// === ROV ===
#include "rov.h"
Rov rov;

#endif
