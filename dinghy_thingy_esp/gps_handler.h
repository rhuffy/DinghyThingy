#ifndef __GPS_HANDLER_H
#define __GPS_HANDLER_H
#include <Arduino.h>

#include <Wire.h>
#include <string.h>

#include "sensor_types.h"

void init_gps();
void get_number_gps(char* data_array,int s, float* j);
void extract_gps_data(char* data_array,GPS_READING_T* data);
void extractGNRMC_gps(GPS_READING_T* data);
GPS_READING_T read_gps();

#endif
