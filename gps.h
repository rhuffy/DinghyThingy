#ifndef __GPS_H
#define __GPS_H

//TODO method headers and constant definitions here.

typedef struct {
  float latitude;
  float longitude;
} GPS_READING_T;


void init_gps();
GPS_READING_T read_gps();

#endif // __GPS_H
