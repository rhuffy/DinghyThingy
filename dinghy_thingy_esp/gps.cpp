#include "gps.h"

void init_gps(){
  gps.begin(9600,SERIAL_8N1,32,33);
}

GPS_READING_T read_gps(){

  GPS_READING_T reading = {
  .latitude = 56.2,
  .longitude = 199.051
  };

  return reading;
}
