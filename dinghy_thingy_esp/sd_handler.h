#ifndef __SD_HANDLER_H
#define __SD_HANDLER_H
#include <Arduino.h>

#include <SPI.h>
#include "FS.h"
#include "SD.h"

#include <string.h>

#define SD_CS 5

#include "fsm.h"
#include "sensor_types.h"

void sd_write(SENSOR_READING_T *data_buffer, int size);
void clear_data_file();
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);
void readFile(fs::FS &fs, const char * path, char* output);
void init_sd();

#endif
