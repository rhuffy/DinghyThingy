#include "fsm.h"

STATE_T current_state;

void advance_state(){
  switch(current_state){
    case
  }
}

void init_state(){
  current_state = STATE_ROOT;
}

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

void enter_state_root(){

}

void update_state_root(){

}


void enter_state_ready(){

}

void update_state_ready(){

}


void enter_state_sense(){

}

void update_state_sense(){

}


void enter_state_writeflash(){

}

void update_state_writeflash(){

}


void enter_state_upload(){

}

void update_state_upload(){

}
