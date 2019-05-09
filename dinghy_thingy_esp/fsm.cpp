#include "fsm.h"

class Button{
  public:
  uint32_t t_of_state_2;
  uint32_t t_of_button_change;
  uint32_t debounce_time;
  uint32_t long_press_time;
  uint8_t pin;
  uint8_t flag;
  bool button_pressed;
  uint8_t state; // This is public for the sake of convenience
  Button(int p) {
  flag = 0;
    state = 0;
    pin = p;
    t_of_state_2 = millis(); //init
    t_of_button_change = millis(); //init
    debounce_time = 10;
    long_press_time = 1000;
    button_pressed = 0;
  }
  void read() {
    uint8_t button_state = digitalRead(pin);
    button_pressed = !button_state;
  }
  int update() {
    read();
    flag = 0;
    switch(state){
      case 0:
        if (button_pressed) {
          state = 1;
          t_of_button_change = millis();
        }
        break;
      case 1:
        if (button_pressed && millis() - t_of_button_change >= debounce_time){
          state = 2;
          t_of_state_2 = millis();
        }
        if (!button_pressed){
          state = 0;
          t_of_button_change = millis();
        }
        break;
      case 2:
        if (button_pressed && millis() - t_of_state_2 >= long_press_time){
          state = 3;
        }
        if (!button_pressed){
          state = 4;
          t_of_button_change = millis();
        }
        break;
      case 3:
        if (!button_pressed){
          state = 4;
          t_of_button_change = millis();
        }
        break;
      case 4:
        if (!button_pressed && millis() - t_of_button_change >= debounce_time){
          state = 0;
          if(millis() - t_of_state_2 < long_press_time){
            flag = 1;
          }else{
            flag = 2;
          }

        }
        if (button_pressed && millis() - t_of_state_2 < long_press_time){
          state = 2;
          t_of_button_change = millis();
        }
        if (button_pressed && millis() - t_of_state_2 >= long_press_time){
          state = 3;
          t_of_button_change = millis();
        }
        break;
    }
    return flag;
  }
};

STATE_T current_state;

SENSOR_READING_T data_buffer[MAX_READINGS+1] = {0};
int data_buffer_index, time_in_ready;

Button record_button(RECORD_BUTTON_PIN); //button object!
Button upload_button(UPLOAD_BUTTON_PIN);

int record_bv, upload_bv;

/**
 * Calls the correct update function based on current state.
 * Should be called once every cycle.
 */
void advance_state(){
  record_bv = record_button.update();
  upload_bv = upload_button.update();
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
  record_bv = 0;
  upload_bv = 0;
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


  if(record_bv == 1){
    set_state(STATE_SENSE);
  }
  else if(upload_bv == 1){
    set_state(STATE_UPLOAD);
  }

}


void enter_state_ready(){
    Serial.println("[READY]");
    time_in_ready = millis();
}

void update_state_ready(){

  if(record_bv == 1){
    set_state(STATE_ROOT);
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
