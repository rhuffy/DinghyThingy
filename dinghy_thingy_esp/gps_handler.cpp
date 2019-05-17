#include "gps_handler.h"


HardwareSerial gps_serial(2); //instantiate approporiate Serial object for GPS
TinyGPSPlus gps;

const int BUFFER_LENGTH = 200;  //size of char array we'll use for
char buffer[BUFFER_LENGTH] = {0}; //dump chars into the

bool valid; //is the data valid

void init_gps(){
  gps_serial.begin(9600,SERIAL_8N1,32,33);
}

GPS_READING_T read_gps(){
  if (gps_serial.available()){
    while (gps_serial.available())
      gps.encode(gps_serial.read());      // Check GPS
  }

  GPS_READING_T data;
  data.latitude = gps.location.lat();
  data.longitude = gps.location.lng();
  data.valid = gps.location.isValid();

  data.day = gps.date.day();
  data.month = gps.date.month();
  data.year = gps.date.year();
  data.hour = gps.time.hour();
  data.minute = gps.time.minute();
  data.second = gps.time.second();
  if(!gps.date.isValid()){
    // dummy date so that post doesnt fail
    data.day = 1;
    data.month = 1;
    data.year = 2019;
  }
  if(!gps.time.isValid()){
    // dummy time so that post doesnt fail
    data.hour = 12;
    data.minute = 0;
    data.second = 0;
  }

  return data;
}
