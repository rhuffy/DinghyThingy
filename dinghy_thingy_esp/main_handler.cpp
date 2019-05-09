#include <Arduino.h>

#include "fsm.h"
#include "gps_handler.h"
#include "imu_handler.h"
#include "sd_handler.h"
#include "wifi_handler.h"

void setup(){
  Serial.begin(115200);
  init_sd();
  init_state();
  init_gps();
  init_imu();
}

void loop(){
  advance_state();
}
