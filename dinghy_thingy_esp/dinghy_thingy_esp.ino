void setup(){
  init_sd();
  init_state();
  init_gps();
  init_imu();
}

void loop(){
  advance_state();
}
