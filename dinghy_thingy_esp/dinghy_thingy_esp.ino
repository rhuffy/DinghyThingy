#include "fsm.h"

void setup(){
  init_state();
  init_gps();
  init_imu()
}

void loop(){
  advance_state();
}
