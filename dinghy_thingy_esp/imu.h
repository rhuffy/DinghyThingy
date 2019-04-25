#ifndef __IMU_H
#define __IMU_H

//TODO method headers and constant definitions here.
#include <string.h>  //used for some string handling and processing.
#include <mpu9255_esp32.h>
//#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display

//TFT_eSPI tft = TFT_eSPI();
MPU9255 imu;

struct IMU_READING_T{
  float x_accel;
  float y_accel;
  float z_accel;
  // change this or add more fields for gyroscope
}


void init_imu();
IMU_READING_T data;
IMU_READING_T read_coords(&data);

#endif // __IMU_H
