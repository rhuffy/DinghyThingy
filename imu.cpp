#include "gps.h"

void init_imu(){
  // stuff that should be in setup()
}

GPS_READING_T read_imu(){


  GPS_READING_T reading = {
  .x_accel = 121.9,
  .y_accel = 0.95,
  .z_accel = 1.331
  };

  return reading;
}
