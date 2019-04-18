#include "gps.h"

void init_gps(){
  // stuff that should be in setup()
}

GPS_READING_T read_gps(){

  GPS_READING_T reading = {
  .latitude = 56.2,
  .longitude = 199.051
  };

  return reading;
}
