#ifndef __GPS_H
#define __GPS_H

#include<Wire.h>
#include<string.h>

#include <SPI.h>
#include <math.h>

HardwareSerial gps(2); //instantiate approporiate Serial object for GPS

const int BUFFER_LENGTH = 200;  //size of char array we'll use for
char buffer[BUFFER_LENGTH] = {0}; //dump chars into the

int lat_deg; //degrees portion of lattitude
float lat_dm; //latitude decimal minutes
char lat_dir; //latitude direction
int lon_deg; //longitude in degrees
float lon_dm; //longitude decimal minutes
char lon_dir; //longitude direction
int year; //year
int month; //month
int day; //day of month
int hour; //hour (24 clock GMT)
int minute; //minute
int second; //second
bool valid; //is the data valid


struct GPS_READING_T{
  float latitude;
  float longitude;
};


void init_gps();

GPS_READING_T data;

read_gps(&data);

#endif // __GPS_H
