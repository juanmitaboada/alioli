#ifndef SHARED_H
#define SHARED_H

#include "alioli.h"
#include "config.h"

FILE serial_stdout;

// === RTC ===
#include <Wire.h>
#include <RTClib.h>
RTC_DS3231 rtc;
long int rtc_millis_offset;

// === Buoy ===
#include "buoy.h"
Buoy buoy;

#endif
