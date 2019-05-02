#ifndef __SD_H
#define __SD_H

#include "FS.h"
#include "SD.h"
#include <SPI.h>

#define SD_CS 5

//TODO method headers and constant definitions here.

void init_sd();
void sd_write(SENSOR_READING_T *data_buffer, int size);

void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void readFile(fs::FS &fs, const char * path);

#endif // __SD_H
