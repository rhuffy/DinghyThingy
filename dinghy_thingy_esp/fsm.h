#ifndef __FSM_H
#define __FSM_H
#include <Arduino.h>

#include "gps_handler.h"
#include "imu_handler.h"
#include "sd_handler.h"
#include "wifi_handler.h"

#define MAX_READINGS 10
#define BOAT_NUMBER 1

#define RECORD_BUTTON_PIN 16
#define UPLOAD_BUTTON_PIN 17

#define LOG_RATE 1000

typedef enum {
  STATE_ROOT,
  STATE_READY,
  STATE_SENSE,
  STATE_WRITEFLASH,
  STATE_UPLOAD
} STATE_T;

void advance_state();
void set_state(STATE_T new_state);
void init_state();

void enter_state_root();
void update_state_root();

void enter_state_ready();
void update_state_ready();

void enter_state_sense();
void update_state_sense();

void enter_state_writeflash();
void update_state_writeflash();

void enter_state_upload();
void update_state_upload();

#endif
