#ifndef __IMU_HANDLER_H
#define __IMU_HANDLER_H
#include <Arduino.h>

#include <SPI.h>
#include <math.h>
#include <mpu9255_esp32.h>

#include "sensor_types.h"

void init_imu();
IMU_READING_T read_imu();

#endif
