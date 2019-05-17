#ifndef __GPS_HANDLER_H
#define __GPS_HANDLER_H
#include <Arduino.h>

#include <Wire.h>
#include <string.h>
#include <TinyGPS++.h>
#include <sys/time.h>

#include "sensor_types.h"

void init_gps();
GPS_READING_T read_gps();

#endif
