#ifndef __IMU_H
#define __IMU_H

//TODO method headers and constant definitions here.

typedef struct {
  float x_accel;
  float y_accel;
  float z_accel;
  // change this or add more fields for gyroscope
} IMU_READING_T;


void init_gps();
IMU_READING_T read_coords();

#endif // __IMU_H
