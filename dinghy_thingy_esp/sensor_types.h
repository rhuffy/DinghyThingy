#ifndef __SENSOR_TYPES_H
#define __SENSOR_TYPES_H

struct GPS_READING_T{
  float latitude; //in degrees
  float longitude; //in degrees
  int year; //year
  int month; //month
  int day; //day of month
  int hour; //hour (24 clock GMT)
  int minute; //minute
  int second; //second
};

struct IMU_READING_T{
  float x_accel;
  float y_accel;
  float z_accel;
  // change this or add more fields for gyroscope
};

typedef struct {
  GPS_READING_T gps;
  IMU_READING_T imu;
  int boat_id;
} SENSOR_READING_T;

#endif
