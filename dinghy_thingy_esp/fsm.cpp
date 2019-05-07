#include "fsm.h"

STATE_T current_state;

SENSOR_READING_T data_buffer[MAX_READINGS+1] = {0};
int data_buffer_index, time_in_ready;

/**
 * Calls the correct update function based on current state.
 * Should be called once every cycle.
 */
void advance_state(){
  switch(current_state){
    case STATE_ROOT:
      update_state_root();
      break;
    case STATE_READY:
      update_state_ready();
      break;
    case STATE_SENSE:
      update_state_sense();
      break;
    case STATE_WRITEFLASH:
      update_state_writeflash();
      break;
    case STATE_UPLOAD:
      update_state_upload();
      break;
  }
}

/**
 * Calls the correct enter_state function for the desired new_state.
 * Re-assignes current_state to the new_state.
 * @param new_state the desired state to enter.
 */
void set_state(STATE_T new_state){
  switch(new_state){
    case STATE_ROOT:
      enter_state_root();
      break;
    case STATE_READY:
      enter_state_ready();
      break;
    case STATE_SENSE:
      enter_state_sense();
      break;
    case STATE_WRITEFLASH:
      enter_state_writeflash();
      break;
    case STATE_UPLOAD:
      enter_state_upload();
      break;
  }

  current_state = new_state;
}


/**
 * Initializes FSM. Should be called once in setup.
 */
void init_state(){
  pinMode(RECORD_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_BUTTON_PIN, INPUT_PULLUP);
  data_buffer_index = 0;
  set_state(STATE_ROOT);
}



// The following functions should not be called directly.
// Use set_state() and update_state() instead.


// enter_state_XXX() is called once when the state is entered.
// update_state_XXX() is called repeatedly while in the associated state.

void enter_state_root(){
  Serial.println("[ROOT]");
}

void update_state_root(){


  if(!digitalRead(RECORD_BUTTON_PIN)){
    set_state(STATE_SENSE);
  }
}


void enter_state_ready(){
    Serial.println("[READY]");
    time_in_ready = millis();
}

void update_state_ready(){

  if(!digitalRead(RECORD_BUTTON_PIN)){
    set_state(STATE_ROOT);
  }
  else if(!digitalRead(UPLOAD_BUTTON_PIN)){
    set_state(STATE_UPLOAD);
  }

  else if(data_buffer_index >= MAX_READINGS-1){
    set_state(STATE_WRITEFLASH);
  }

  else if(millis() - time_in_ready > LOG_RATE){
    set_state(STATE_SENSE);
  }
}


void enter_state_sense(){
  Serial.println("[SENSE]");
}

void update_state_sense(){
  // get sensor readings

  SENSOR_READING_T current_reading = {
    .gps = read_gps(),
    .imu = read_imu(),
    .boat_id = BOAT_NUMBER
  };

  // add data to buffer
  Serial.print("Save data at: ");
  Serial.println(data_buffer_index);
  data_buffer[data_buffer_index++] = current_reading;

  set_state(STATE_READY);
}


void enter_state_writeflash(){
  Serial.println("[WRITEFLASH]");
}

void update_state_writeflash(){


  data_buffer_index = 0;
  sd_write(data_buffer, MAX_READINGS);

  set_state(STATE_READY);
}


void enter_state_upload(){
  Serial.println("[UPLOAD]");
}

void update_state_upload(){

  // char dat[] = "22, 2019-04-24T13:29:13.5, 42.357, -71.091, .01, .01, 59.04\n227,  2019-02-23T13:32:16.5, 42.355, -71.094, .1, .1, 589.09\n22, 2019-04-24T13:32:15.5, 42.356, -71.094, .13, .13, 59.04\n22, 2019-04-24T13:35:16.8, 42.353, -71.100, .3, .3, 59.04";
  wifi_connect();
  char dat[5000];
  readFile(SD, "/data.txt", dat);
  Serial.println(dat);
  //readFile()
  char response[1000];
  send_info(dat,response);
  if(!strcmp(response,"1")){
    set_state(STATE_ROOT);
  }
  else{
    Serial.println("Error uploading");
  }
}
