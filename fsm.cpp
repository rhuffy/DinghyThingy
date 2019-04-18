#include "fsm.h"


STATE_T current_state;

SENSOR_READING_T data_buffer[MAX_READINGS+1] = {0};
int data_buffer_index;



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
  data_buffer_index = 0;
  set_state(STATE_ROOT);
}



// The following functions should not be called directly.
// Use set_state() and update_state() instead.


// enter_state_XXX() is called once when the state is entered.
// update_state_XXX() is called repeatedly while in the associated state.

void enter_state_root(){

}

void update_state_root(){


  if(/*Record Button Pressed*/){
    set_state(STATE_SENSE);
  }
}


void enter_state_ready(){

}

void update_state_ready(){

  if(/*Record Button Pressed*/){
    set_state(STATE_ROOT);
  }
  else if(/*Upload Button Pressed*/){
    set_state(STATE_UPLOAD);
  }

  else if(data_buffer_index >= MAX_READINGS){
    set_state(STATE_WRITEFLASH);
  }

  else if(time_in_ready > LOG_RATE){
    set_state(STATE_SENSE);
  }
}


void enter_state_sense(){

}

void update_state_sense(){
  // get sensor readings

  SENSOR_READING_T current_reading = {
    .gps = read_gps(),
    .imu = read_imu(),
    .time = now() //from time library
  };

  // add data to buffer
  data_buffer[data_buffer_index++] = current_reading;

  set_state(STATE_READY);
}


void enter_state_writeflash(){

}

void update_state_writeflash(){


  data_buffer_index = 0;
  sd_write(data_buffer, MAX_READINGS);

  set_state(STATE_READY);
}


void enter_state_upload(){

}

void update_state_upload(){
  // try to upload

  if(/*Upload successful*/){
    set_state(STATE_ROOT);
  }
}
